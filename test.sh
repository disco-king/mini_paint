#!/usr/bin/bash

./paint test_in > test1
./their test_in > test2
diff test1 test2 > outfile
#rm test1 test2
echo "outfile:"
cat -e outfile