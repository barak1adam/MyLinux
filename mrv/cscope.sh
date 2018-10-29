echo 1
find . -name "*.[ch]" > cscope.files
find . -name "*.def" >> cscope.files
#find . -follow -name "*.[ch]" > cscope.files
echo 2
cscope -b
echo 3
