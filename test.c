#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>

#include "database.c"

/*
 * Test file for back-end logic (this does NOT test the user inputs, only manipulating the database)
 *
 * Contains functions for creating test Record structs to a specified number, as well as a few tests for making sure the
 * database is being updated properly.
 *
 * THIS ASSUMES THAT THE db_load_csv() and db_read_csv() FUNCTIONS ARE LOCATED INSIDE THE database.c FILE! So you may want
 * to move these there for testing if you haven't already.
 *
 * Some other files that are necessary for this to work:
 * 1. database.h 
 * 2. test-database-read.csv (contains some test records)
 * 3. build_test
 *
 * In order to run the test, you can do ./build_test (make sure to chmod +x it first)
 *
 * Also this test script is kind of wonky (I tried my best) so let me know if there's any improvements that can be made!
 */


/* Test that two values are equal 
 */
void testEquals(int expected, int actual, const char *test_name) {
	printf("----------\n");
	if (expected == actual) {
		printf("Test '%s' passed.\n", test_name);
	} else {
		printf("Test '%s' failed. Got %d instead. Expected: %d.\n",test_name, actual, expected);
	}
}

/*
 * Record creation
 */
Record createItem(const char *handle, const char *comment, long unsigned int followers, long unsigned int time) {
	Record newRecord;
	strcpy(newRecord.handle, handle);
	strcpy(newRecord.comment, comment);
	
	newRecord.followers = followers;
	newRecord.dateModified = time;
	return newRecord;
}

/*
 * Create an array of Record structs with the form:
 *
 * handle = test_k, comment = test_comment_k, followers = k, dateModified = startTime + k
 *
 * Takes as parameters the number of structs, and the startTime
 */ 
Record* createRecords(int num, int startTime) {
	Record* records = malloc(num * sizeof(Record)); // Initialize array of Record structs

	for (int i =0; i < num; i++) {
		char handle[32];
		char comment[64];
		sprintf(handle, "test_%d", i);
		sprintf(comment, "test_comment_%d", i);
		records[i] = createItem(handle, comment, (unsigned long int) i, (unsigned long int) (startTime + i));
	}	
	return records;
}

/*
 * Database creation. Checks that:
 * 1. Capacity and size fields are updated properly.
 * 2. That the Record pointer is there.
 */
int createDatabase(Database *db) {
	if (db->data != NULL && db->capacity == 4 && db->size == 0) {
		return 0;
	} else {
		if (db->data == NULL) {
			printf("Pointer to array was not properly initialized.");
			return 2; // Not properly initialized
		}
		printf("Actual capacity: %d, actual size: %d", db->capacity, db->size);
		return 1; // Something went wrong with capacity and size
	}
}

/*
 * Check that db_append() properly updates size, original capacity, and is resized accordingly
 */
int appendDatabase(Database *db, Record const *item, char expectedHandle[]) {
	int originalCapacity = db->capacity;
	int originalSize = db->size;
	
	db_append(db, item);
	if (originalCapacity > originalSize) {
		if (originalCapacity != db->capacity || originalSize != (db->size - 1)) {
			printf("Something went wrong with capacity and size fields.");
			return 1;
		}
	} else if (originalCapacity == originalSize) {
		if ((originalCapacity * 2) != db->capacity || originalSize != (db->size - 1)) {
			printf("Something went wrong with capacity and size fields.");
			return 2;
		}
	}
	// Check that handles are the same
	int location = (db->size)-1;
	if (strcmp((db->data[location]).handle, expectedHandle) != 0) {
		printf("Item counld not be found in array.");
	       	return 3;	
	}
	return 0;
}


/*
 * Print single record in the database
 */
void printRecord(Record *record) {
	printf("%s, %s, %lu, %lu\n", record->handle, record->comment, record->followers, record->dateModified); // Just to read the record 
}

void printDatabase(Database *db) {
	printf("Currently in database: \n");
	for (int i = 0; i < db->size; i++) {
		Record record = db->data[i];
		Record* record_ptr = &record;
		printRecord(record_ptr);
	}
}


/* A very long and wonky main() function. Any suggestions for improvement would be appreciated ;-;
 */
int main(void) {
	// Building database and records
	Database newDatabase = db_create(); // Initializing a database
	Database *db = &newDatabase;
	int curTime = (int) time(NULL);
	Record* records = createRecords(5, curTime); // Will test adding 5 Record structs

	// Test functions
	testEquals(0, createDatabase(db), "db_create()"); // Test that database is correctly initialized
	for (int i = 0; i < 5; i++) {
		// Need to convert each element in array to a pointer to that element instead
		Record curRecord = records[i];
		Record* ptr_record = &curRecord;
		testEquals(0, appendDatabase(db, ptr_record, records[i].handle),"db_append()"); // Test that db_append() is functioning correctly	
		printDatabase(db);
	
	}
	
	// Initialize pointers to test records
	Record* record_ptr_0 = db_index(db, 0);
	Record* record_ptr_4 = db_lookup(db, "test_4");
	Record* record_ptr_5 = db_lookup(db, "test_5"); // Not in database
	testEquals(0, record_ptr_0->followers, "db_index()"); // Use follower fields to check that the correct pointer was returned for db_index() 
	testEquals(4, record_ptr_4->followers, "db_lookup() - handle in database"); // Use followers fields to check that the correct pointer was returned for db_lookup()
	
	int checkNull1 = (NULL == record_ptr_5); // This should be null
	testEquals(1, checkNull1, "db_lookup() - handle not in database"); // Test that db_lookup() returns null if the handle is not in the database
	
	Record parsedRecord = parse_record("parsed,11,11,parsed-comment");
	Record *parsedRecord_ptr = &(parsedRecord);
	testEquals(11, parsedRecord.followers, "parse_record() test"); // Test that parseRecord correctly returns a Record struct
	printRecord(parsedRecord_ptr);

	printf("----------\n");

	db_load_csv(db, "test-database-read.csv");
	printf("Testing loading the CSV file\n"); // Need a way to test equality properly :]
	printDatabase(db);

	printf("----------\n");
	
	db_write_csv(db, "test-database-write.csv");
	printf("Testing writing into CSV file. Note that this file should be created if it does not exist already. Content should be same as what was read from db_load_csv()\n");
	
	FILE *fileSource = fopen("test-database-write.csv", "r");

	char *line = NULL;
	size_t len = 0;

	while (getline(&line, &len, fileSource) != -1) {
		fputs(line, stdout);
	}

	free(line);
	fclose(fileSource);

	db_free(db);
	int checkNull2 = (NULL == db->data); // This should be null - database is now unusable
	testEquals(1, checkNull2, "db_free()"); // Test that db_free() will cause the data pointer to be null

	// Free allocated space
	free(records);
	db = NULL; // Dangling pointers
	records = NULL;
}
