files=""
for file in "./"*.c
do
    files="$file $files"
done
echo "Files added to gcc $files"
gcc $files -o main  -lSDL2 -lSDL2_image -lm && ./main $1

