# nyancodec

Main code is stored in the header nyancodec.hpp. Each message is split into blocks, fields, subfields and items. There are two main objects: cat and msg. Cat contains the information about message encoding/decoding, message composition, contents, etc. It looks like this cat > field_spec > subfield_spec > item_spec. msg serves as a container for the decoded message. The hierachy of the message is as follows: msg>block > field > subfield > item(contains the full hex data string) > val(contains the decoded value). 

Run the Makefile to compile the testing binary. Testing binary grabs the binary input from the sample_message.bin and the CAT specs stored in the cat folder and prints the contents in the console.

Additional utility functions and variables are stored in util.hpp. These are used to perform bitwise operations, perform various type conversions, etcetera.
