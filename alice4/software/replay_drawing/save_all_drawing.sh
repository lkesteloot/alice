for i in logo jello bounce buttonfly insect
do 
    (cd $i ; SAVE_DRAWING_FILENAME=$i""_drawing.txt SAVE_DRAWING_LAST_FRAME=200 make run )
done

for i in arena
do
    (cd $i ; SAVE_DRAWING_FILENAME=$i""_drawing.txt SAVE_DRAWING_LAST_FRAME=200 make run_no_network )
done
