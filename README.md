To run the program, simply do ./build_test and then ./test

A note that the test.c file assumes that all db_load_csv(), parse_record(), and db_write_csv() are located inside of database.c. It also reads from the file called "test-database-read.csv" so make sure that it's in the same directory. 

TBA:
1. A better way to ensure that the test works without needing to fiddle with moving functions 
2. Testing equality of CSV files automatically

Suggestions and improvements very much appreciated I have no idea what I'm doing in C :'))
