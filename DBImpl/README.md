##############################
README DATABASE IMPLEMENTATION
##############################
Implemented by: DRomanAvid
--------------------------

Compilation
-----------
make all


Execution
---------

External Sorting Test:
dbi 0 <input_file> <output_file> <memory_buffer_in_MB>

Buffer Manager Test:
dbi 1 <database_file> <pages_on_disk> <memory_size_pages> <no_threads>

Segment Manager Test:
dbi 2 <database_file> <page_size_per_slotted_page>

B-Tree-Test:
dbi 3 <database_file> <no_btree_items>
