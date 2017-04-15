for i in logo jello bounce buttonfly insect
do
    echo $i

    rm frame*ppm
    replay_drawing/replay_drawing_reference $i/$i""_drawing.txt
    ffmpeg -r 60 -f image2 -s 800x480 -i frame%04d.ppm -vcodec libx264 -crf 5  -pix_fmt yuv420p $i""_fractional.mp4

    rm frame*ppm
    SNAP_VERTICES=1 replay_drawing/replay_drawing_reference $i/$i""_drawing.txt
    ffmpeg -r 60 -f image2 -s 800x480 -i frame%04d.ppm -vcodec libx264 -crf 5  -pix_fmt yuv420p $i""_snapped.mp4
done
