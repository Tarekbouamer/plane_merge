#!/bin/bash

Workspace=/media/loc/ssd_1tb/multi_view_training_dslr_undistorted
Weights=thirdparty/MiDaS/weights/dpt_large-midas-2f21e586.pt

Scenes="pipes office courtyard delivery_area electro facade kicker meadow playground relief relief_2 terrace terrains"

for scene in $Scenes;
do
    echo "Midas run :: ${scene}"

    DATASET_PATH="$Workspace/$scene"

    src_dir=$DATASET_PATH/dense/images/dslr_images_undistorted/
    dst_dir=$DATASET_PATH/dense/stereo/mono_depth
    
    # echo $dst_dir
    rm -rf $dst_dir
    mkdir -p $dst_dir
    
    # estimate mono depth
    python thirdparty/MiDaS/run.py -i $src_dir -o $dst_dir -m $Weights
done 
