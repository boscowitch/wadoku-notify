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


void init_db(const char* path) {

	int op = sqlite3_open(":memory:", &db); //op = sqlite3_open_v2("wadoku.sqlite3",&db,SQLITE_OPEN_READONLY|SQLITE_OPEN_NOMUTEX,0);

	op = sqlite3_exec(db,"PRAGMA read_uncommitted = True;",0,0,0);
	if(op) {
		notify("read_uncommitted",sqlite3_errmsg(db));
	}


	if(path != NULL) {
		char* sql = (char*) malloc(36+ strlen(path));
		sprintf( sql,"ATTACH '%s/wadoku.sqlite3' AS wadoku",path);
		op = sqlite3_exec(db,sql,0,0,0);
		free(sql);
	}
	else {
		op = sqlite3_exec(db,"ATTACH 'wadoku.sqlite3' AS wadoku",0,0,0);
	}

	if(op) {
		notify("sqlite3_exec",sqlite3_errmsg(db));
	}

	/* Old version without index
	op = sqlite3_exec(db,"CREATE TABLE ram AS SELECT * FROM wadoku.entries",0,0,0);
	if(op) {
		notify("sqlite3_exec",sqlite3_errmsg(db));
	} */

	op = sqlite3_exec(db,"CREATE VIRTUAL TABLE ram USING fts3(japaneseins,german,tokenize=simple)",0,0,0);
	if(op) {
		notify("sqlite3_exec",sqlite3_errmsg(db));
	}

	op = sqlite3_exec(db,"INSERT INTO ram ( japaneseins ,german ) SELECT ((japanese || ' ')|| reading) AS japaneseins, german FROM wadoku.entries order by id asc",0,0,0);
	if(op) {
		notify("sqlite3_exec_insert_into_ram",sqlite3_errmsg(db));
	}


}

bool db_search(const char* str) {
    int op=0;

	const char* SQL = "select japaneseins,german from ram where japaneseins match ? order by docid asc limit 1";
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
            if(strcmp(last,stem_transformation[i*3])) {

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
    return -1;
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
	old = malloc(strlen(str)+1);
	strcpy(old,str);
	//end ignore unchanged str

	if(db_search(str)){
		return;
	} else {
	    search_for_stems(str);
	}

}
