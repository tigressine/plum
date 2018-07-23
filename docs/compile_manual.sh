STEM="MANUAL"

#xelatex $STEM.tex

for FILE in *
do
    if echo $FILE | grep -q $STEM
    then
        if $FILE -neq $STEM.pdf && $FILE -neq $STEM.tex
        then
            rm $FILE
        fi
    fi
done
