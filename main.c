#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <pthread.h>

#define Logs

void do_exit(PGconn *conn) {
    PQfinish(conn);
    exit(1);
}

void *pg_thread(void *arg) {
    int *startId = (int *) arg;
    PGconn *conn = PQconnectdb(
            "hostaddr = '127.0.0.1' port = '5432' dbname = 'postgres' user = 'postgres' password = 'password' connect_timeout = '10'");

    if (PQstatus(conn) != CONNECTION_OK) {

        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        do_exit(conn);
    }

    PGresult *res;

    for (int i = *startId; i < (*startId) + 100000; ++i) {
        if (i % 10000 == 0) {
            printf("Thread with startId: %d just did 10000 inserts\n", *startId);
        }
        char request[100];
        sprintf(request, "INSERT INTO Cars VALUES(%d,'Audi',52642)", i);
        res = PQexec(conn, request);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            PQclear(res);
            do_exit(conn);
        }
        PQclear(res);
    }


    PQfinish(conn);
}

int runCommand(PGconn *connection, const char * command, int printCommand, int printErrors){
    if(printCommand == 1){
        printf(command);
        printf("\n");
    }
    PGresult *res = PQexec(connection, command);
    int status = PQresultStatus(res);
    PQclear(res);

    if(printErrors && status != PGRES_COMMAND_OK){
        fprintf(stderr, "Command error: %s\n", PQerrorMessage(connection));
    }
    return status;
}

int main() {
    int numOfThreads = 10;

    pthread_t *ptr = malloc(sizeof(pthread_t) * numOfThreads);

    int args[numOfThreads];

    //region Database connection

    PGconn *conn = PQconnectdb("hostaddr = '127.0.0.1' port = '5432' dbname = 'tweets' user = 'postgres' password = 'postgres' connect_timeout = '10'");

    if (PQstatus(conn) != CONNECTION_OK) {

        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        do_exit(conn);
    }

    printf("Database connected successfully\n");

    //endregion

    //region Drop and create tables
    printf("Dropping all tables.\n");
    runCommand(conn, "DROP TABLE IF EXISTS accounts, tweet_mentions, tweets, countries, tweet_hashtags, hashtags CASCADE", 1, 1);

    printf("Creating table accounts.\n");
    runCommand(conn, "CREATE TABLE accounts("
                         "id BIGINT PRIMARY KEY, "
                         "screen_name VARCHAR(200), "
                         "name VARCHAR(200), "
                         "description TEXT, "
                         "followers_count INTEGER, "
                         "friends_count INTEGER, "
                         "statuses_count INTEGER)", 1, 1);

    printf("Creating table tweet_mentions.\n");
    runCommand(conn, "CREATE TABLE tweet_mentions ("
                         "id INTEGER PRIMARY KEY, "
                         "account_id BIGINT, "
                         "tweet_id VARCHAR(20))", 1, 1);

    printf("Creating table tweets.\n");
    runCommand(conn, "CREATE TABLE tweets("
                     "id VARCHAR(20) PRIMARY KEY, "
                     "content TEXT, "
                     "location GEOMETRY(POINT,4326), "
                     "retweet_count INTEGER, "
                     "favourite_count INTEGER, "
                     "happened_at TIMESTAMP WITH TIME ZONE, "
                     "author_id BIGINT, "
                     "country_id INTEGER, "
                     "parent_id VARCHAR(20))", 1, 1);

    printf("Creating table countries.\n");
    runCommand(conn, "CREATE TABLE countries("
                     "id INTEGER PRIMARY KEY, "
                     "code VARCHAR(2), "
                     "name VARCHAR(200))", 1, 1);

    printf("Creating table tweet_hashtags.\n");
    runCommand(conn, "CREATE TABLE tweet_hashtags("
                     "id INTEGER PRIMARY KEY, "
                     "hashtag_id INTEGER, "
                     "tweet_id VARCHAR(20))", 1, 1);

    printf("Creating table hashtags.\n");
    runCommand(conn, "CREATE TABLE hashtags("
                     "id INTEGER PRIMARY KEY, "
                     "value TEXT)", 1, 1);

    //endregion

    //region Create foreign keys
    printf("Adding foreign keys.\n");
    runCommand(conn, "ALTER TABLE tweet_mentions "
                     "ADD CONSTRAINT fk_tweet_mentions_accounts "
                     "FOREIGN KEY (account_id) "
                     "REFERENCES accounts (id);", 1, 1);

    runCommand(conn, "ALTER TABLE tweet_mentions "
                     "ADD CONSTRAINT fk_tweet_mentions_tweets "
                     "FOREIGN KEY (tweet_id) "
                     "REFERENCES tweets (id);", 1, 1);

    runCommand(conn, "ALTER TABLE tweets "
                     "ADD CONSTRAINT fk_tweets_accounts "
                     "FOREIGN KEY (author_id) "
                     "REFERENCES accounts (id);", 1, 1);

    runCommand(conn, "ALTER TABLE tweets "
                     "ADD CONSTRAINT fk_tweets_tweets "
                     "FOREIGN KEY (parent_id) "
                     "REFERENCES tweets (id);", 1, 1);

    runCommand(conn, "ALTER TABLE tweets "
                     "ADD CONSTRAINT fk_tweets_countries "
                     "FOREIGN KEY (country_id) "
                     "REFERENCES countries (id);", 1, 1);

    runCommand(conn, "ALTER TABLE tweet_hashtags "
                     "ADD CONSTRAINT fk_tweet_hashtags_tweets "
                     "FOREIGN KEY (tweet_id) "
                     "REFERENCES tweets (id);", 1, 1);

    runCommand(conn, "ALTER TABLE tweet_hashtags "
                     "ADD CONSTRAINT fk_tweet_hashtags_hashtags "
                     "FOREIGN KEY (hashtag_id) "
                     "REFERENCES hashtags (id);", 1, 1);

    //endregion




//    PGresult *res;

//    PGresult *res = PQexec(conn, "DROP TABLE IF EXISTS Cars");
//
//    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//        PQclear(res);
//        do_exit(conn);
//    }
//
//    PQclear(res);

//    printf("Database cleared successfully\n");

//    res = PQexec(conn, "CREATE TABLE Cars(Id INTEGER PRIMARY KEY, Name VARCHAR(20), Price INT)");
//
//    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
//        PQclear(res);
//        do_exit(conn);
//    } else {
//        printf("Created table cars successfully\n");
//    }
//
//    PQclear(res);
//    PQfinish(conn);
//
//    for (int i = 0; i < numOfThreads; i++) {
//        args[i] = i * 100001;
//        pthread_create(&ptr[i], NULL, pg_thread, (void *) &args[i]);
//    }
//    for (int i = 0; i < numOfThreads; i++)
//        pthread_join(ptr[i], NULL);


    return 0;
}