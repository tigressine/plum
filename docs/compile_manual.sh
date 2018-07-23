# Compile the manual into a pdf and delete all the leftover garbage.

# Change this variable to change the name of the input and result.
STEM="MANUAL"

# Compile the tex file.
xelatex $STEM.tex

# Check all files in the directory. There's probably a cleaner, more "shell"
# way to do this. Shell is rough to write...
for FILE in *
do
    # If the file is related to the manual, then check if it is the input
    # or the output. If it's neither, eliminate it.
    if echo $FILE | grep -q $STEM
    then
        if [ "$FILE" != "$STEM.pdf" ] && [ "$FILE" != "$STEM.tex" ]
        then
            rm $FILE
        fi
    fi
done
