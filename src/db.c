#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include"sqlite3.h"
#include <stdio.h>
#include"wordstem.h"

#define SQLITE_ENABLE_FTS3
#define SQLITE_ENABLE_FTS3_PARENTHESIS

void notify(const char* ctitle, const char* ctext);

static sqlite3* db = 0;
static sqlite3_stmt * stm = 0;

int loadDb(sqlite3 *pInMemory, sqlite3 *pfileDB){
  int rc;                   /* Function return code */
  sqlite3_backup *pBackup;  /* Backup object used to copy data */


    pBackup = sqlite3_backup_init(pInMemory, "main", pfileDB, "main");
    if( pBackup ){
      (void)sqlite3_backup_step(pBackup, -1);
      (void)sqlite3_backup_finish(pBackup);
    }
    rc = sqlite3_errcode(pInMemory);

  return rc;
}


void init_db(const char* path,bool mm) {

	int op; //op = sqlite3_open_v2("wadoku.sqlite3",&db,SQLITE_OPEN_READONLY|SQLITE_OPEN_NOMUTEX,0);

	if(path != NULL) {
		char* sql = (char*) malloc(36+ strlen(path));
		sprintf( sql,"%s/wadoku.sqlite3",path);
		op = sqlite3_open(sql, &db);
		free(sql);
	}
	else {
		op = sqlite3_open("wadoku.sqlite3", &db);
	}

	if(op) {
		notify("sqlite3_open",sqlite3_errmsg(db));
	}

    if(mm) {
        sqlite3 *memory_db;

        op = sqlite3_open(":memory:",&memory_db);
        if(op) {
            notify("sqlite3_open","failed to open memory database, could lead to slower search speed!");
            op = 0;
        }

        if( SQLITE_OK  == loadDb(memory_db,db)) {
            sqlite3_close(db);
            db=memory_db;
        } else {
            notify("loadDb",sqlite3_errmsg(memory_db));
            sqlite3_close(memory_db);
        }
    }


	op = sqlite3_exec(db,"PRAGMA read_uncommitted = True;",0,0,0);
	if(op) {
		notify("read_uncommitted",sqlite3_errmsg(db));
	}

	/* Old version without index
	op = sqlite3_exec(db,"CREATE TABLE ram AS SELECT * FROM wadoku.entries",0,0,0);
	if(op) {
		notify("sqlite3_exec",sqlite3_errmsg(db));
	} */

}

bool db_search(const char* str) {
    int op=0;

	const char* SQL = "select japaneseins,german from indexed_entries where japaneseins match ? order by docid asc limit 1";
	op = sqlite3_prepare_v2(db,SQL,strlen(SQL),&stm,0);
	if(op) {
		notify("sqlite3_prepare_v2",sqlite3_errmsg(db));
	}

	char buffer[2048];
	sprintf(buffer,"%s",str);
	sqlite3_bind_text(stm,1,buffer,-1,SQLITE_STATIC);
	int res = sqlite3_step(stm);

	if(res==SQLITE_ROW) {
		const char* title = (const char*) sqlite3_column_text(stm,0);
		const char* text = (const char*) sqlite3_column_text(stm,1);
		notify( title, text);
		sqlite3_reset(stm);
		return true;
    }
    sqlite3_reset(stm);
	return false;
}

int search_for_stems(const char* str) {
    int i=0;
    for(i;i<TRANSFORMATION_ENTRYS_COUNT;i++) {
        if(strlen(str)>strlen(stem_transformation[i*3]) ) {
            char* last= (char*) &str[ strlen(str)- strlen(stem_transformation[i*3]) ];
            if(strcmp(last,stem_transformation[i*3]) == 0) {

                int stem_lenght = strlen(str) - strlen(last);
                int new_suffix_lenght = strlen(stem_transformation[i*3+1]);

                char* temp = (char*) calloc( (stem_lenght + new_suffix_lenght + 1),sizeof(char) );


                strncpy(temp,str,stem_lenght);
                strncpy((char*)&temp[stem_lenght], stem_transformation[i*3+1], new_suffix_lenght);

                if(db_search(temp)) {
                    free(temp);
                    return -1;
                }
                free(temp);
            }

        }
    }
    return -2;
}

void lookup(const char* str) {
	//ignore unchanged str
	static char* old = 0;
	if(old) {
		if(!strcmp(str,old)) {
			return;
		}
	}
	free(old);
	old = calloc(strlen(str)+1,sizeof(char));
	strcpy(old,str);
	//end ignore unchanged str

	if(db_search(str)){
		return;
	} else if(search_for_stems(str) == -1) {
		return;
	} else {
		char *temp;
		temp = (char*) calloc(strlen(str) + strlen("…") + 1,sizeof(char));
		strcpy(temp,str);
		strcpy(&temp[strlen(temp)],"…");
		if(!db_search(temp))
		{
			strcpy(temp,"…");
			strcpy(&temp[strlen("…")],str);
			db_search(temp);
		}
		free(temp);
	}

}
