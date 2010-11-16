/*
 * This file implements a tokenizer for fts3 based on the MeCab library.
 */



#include<assert.h>
#include<stdlib.h>
#include<string.h>
#include"sqlite3.h"
#include<mecab.h>



typedef struct MecabTokenizer {
    sqlite3_tokenizer base;
    mecab_t *mecab;
} MecabTokenizer;

typedef struct MecabCursor {
    sqlite3_tokenizer_cursor base;
    mecab_node_t *node;
    char *buf;
    int buflen;
    int offset;
    int pos;
} MecabCursor;

/*
 * Create a new tokenizer instance.
 */
static int mecabCreate(
    int argc,                       /* Number of entries in argv[] */
    const char * const *argv,       /* Tokenizer creation arguments */
    sqlite3_tokenizer **ppTokenizer /* OUT: Created tokenizer */
){
    MecabTokenizer *p;
    mecab_t *mecab;
    p = (MecabTokenizer*) malloc(sizeof(MecabTokenizer));
    if(p == NULL) {
        return SQLITE_NOMEM;
    }
    memset(p, 0, sizeof(MecabTokenizer));

    p->mecab = mecab_new(argc, (char**)argv);
    if (p->mecab == NULL) {
        return SQLITE_ERROR;
    }

    *ppTokenizer = (sqlite3_tokenizer *)p;

    return SQLITE_OK;
}

/*
 * Destroy a tokenizer
 */
static int mecabDestroy(sqlite3_tokenizer *pTokenizer){
    MecabTokenizer *p = (MecabTokenizer *)pTokenizer;
    mecab_destroy(p->mecab);
    free(p);
    return SQLITE_OK;
}

/*
 * Prepare to begin tokenizing a particular string.  The input
 * string to be tokenized is pInput[0..nBytes-1].  A cursor
 * used to incrementally tokenize this string is returned in
 * *ppCursor.
 */
static int mecabOpen(
    sqlite3_tokenizer *pTokenizer,      /* The tokenizer */
    const char *pInput,                 /* Input string */
    int nInput,                         /* Length of pInput in bytes */
    sqlite3_tokenizer_cursor **ppCursor /* OUT: Tokenization cursor */
) {
    MecabTokenizer *p = (MecabTokenizer *)pTokenizer;
    MecabCursor *pCsr;
    mecab_node_t *node;

    *ppCursor = 0;

    pCsr = (MecabCursor *)malloc( sizeof(MecabCursor));
    if(pCsr == NULL){
        return SQLITE_NOMEM;
    }
    memset(pCsr, 0, sizeof(MecabCursor));

    node = mecab_sparse_tonode2(p->mecab, pInput, strlen(pInput)+1);
    if (node == NULL) {
        return SQLITE_ERROR;
    }
#define DEFAULT_CURSOR_BUF 256
    pCsr->node = node;
    pCsr->buf = malloc(DEFAULT_CURSOR_BUF);
    pCsr->buflen = DEFAULT_CURSOR_BUF;
    pCsr->offset = 0;
    pCsr->pos = 0;

    *ppCursor = (sqlite3_tokenizer_cursor *)pCsr;
    return SQLITE_OK;
}

/*
 * Close a tokenization cursor previously opened
 * by a call to mecabOpen().
 */
static int mecabClose(sqlite3_tokenizer_cursor *pCursor){
    MecabCursor *pCsr = (MecabCursor *)pCursor;
    free(pCsr->buf);
    free(pCsr);
    return SQLITE_OK;
}

/*
 * Extract the next token from a tokenization cursor.
 */
static int mecabNext(
    sqlite3_tokenizer_cursor *pCursor,/* Cursor returned by mecabOpen */
    const char **ppToken,   /* OUT: *ppToken is the token text */
    int *pnBytes,           /* OUT: Number of bytes in token */
    int *piStartOffset,     /* OUT: Starting offset of token */
    int *piEndOffset,       /* OUT: Ending offset of token */
    int *piPosition         /* OUT: Position integer of token */
){
    mecab_node_t *node;
    int nlen;

    MecabCursor *pCsr = (MecabCursor *)pCursor;
    node = pCsr->node;
    while (node->next != NULL && node->length == 0) {
        node = node->next;
    }

    nlen = node->length;
    if (node->length > pCsr->buflen) {
        char *buf = realloc(pCsr->buf, node->length + 1);
        pCsr->buf = buf;
        pCsr->buflen = node->length;
    }
    strncpy(pCsr->buf, node->surface, node->length);
    pCsr->buf[node->length] = '\0';

    *ppToken = pCsr->buf;
    *pnBytes = node->length;
    *piStartOffset = pCsr->offset;
    *piEndOffset = pCsr->offset + node->length;
    *piPosition = pCsr->pos++;

    if (node->next == NULL) {
      return SQLITE_DONE;
    }
    pCsr->node = node->next;
    pCsr->offset += node->rlength;
    return SQLITE_OK;
}

/*
 * The set of routines that implement the MeCab tokenizer
 */
static const sqlite3_tokenizer_module mecabTokenizerModule = {
    0,                          /* iVersion */
    mecabCreate,                /* xCreate  */
    mecabDestroy,               /* xCreate  */
    mecabOpen,                  /* xOpen    */
    mecabClose,                 /* xClose   */
    mecabNext,                  /* xNext    */
};


static int registerTokenizer(
    sqlite3 *db,
    char *zName,
    const sqlite3_tokenizer_module *p
){
    int rc;
    sqlite3_stmt *pStmt;
    const char zSql[] = "SELECT fts3_tokenizer(?, ?)";

    rc = sqlite3_prepare_v2 (db, zSql, -1, &pStmt, 0);

    if( rc!=SQLITE_OK ){
        return rc;
    }

    sqlite3_bind_text(pStmt, 1, zName, -1, SQLITE_STATIC);
    sqlite3_bind_blob(pStmt, 2, &p, sizeof(p), SQLITE_STATIC);
    sqlite3_step(pStmt);

    return sqlite3_finalize(pStmt);
}


