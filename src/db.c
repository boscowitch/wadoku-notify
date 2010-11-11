#include<stdlib.h>
#include<string.h>
#include"sqlite3.h"
#include <stdio.h>

void notify(const char* ctitle, const char* ctext);

static sqlite3* db = 0;
static sqlite3_stmt * stm = 0;


void init_db() {
	
	int op = sqlite3_open(":memory:", &db); //op = sqlite3_open_v2("wadoku.sqlite3",&db,SQLITE_OPEN_READONLY|SQLITE_OPEN_NOMUTEX,0);
	
	op = sqlite3_exec(db,"PRAGMA read_uncommitted = True;",0,0,0);
	if(op) {
		notify("read_uncommitted",sqlite3_errmsg(db));
	}
	
	op = sqlite3_exec(db,"ATTACH 'wadoku.sqlite3' AS wadoku",0,0,0);

	if(op) {
		notify("sqlite3_exec",sqlite3_errmsg(db));
	}
	
	/*
	op = sqlite3_exec(db,"PRAGMA max_page_count = 99999999999",0,0,0);
	if(op) {
		notify("read_uncommitted",sqlite3_errmsg(db));
	}*/

	op = sqlite3_exec(db,"CREATE TABLE ram AS SELECT * FROM wadoku.entries",0,0,0);
	if(op) {
		notify("sqlite3_exec",sqlite3_errmsg(db));
	}
		
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
	
	int op=0;
		
	const char* SQL = "select (japanese || ' ')|| reading,german from ram where japanese like ? order by LENGTH(japanese_stripped) asc limit 1";
	op = sqlite3_prepare_v2(db,SQL,strlen(SQL),&stm,0);
	if(op) {
		notify("sqlite3_prepare_v2",sqlite3_errmsg(db));
	}


	char buffer[2048];
	sprintf(buffer,"%%%s%%",str);
	sqlite3_bind_text(stm,1,buffer,-1,SQLITE_STATIC);
	int res = sqlite3_step(stm);

	if(res==SQLITE_ROW) {
		const char* title = (const char*)sqlite3_column_text(stm,0);
		const char* text = (const char*)sqlite3_column_text(stm,1);
		notify(title,text);
	}
	
	sqlite3_reset(stm);
	
}
