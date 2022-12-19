#!/bin/bash

MODEL_PATH=thirdparty/PlaneRecNet/
Workspace=/media/dl/ssd_1tb/multi_view_training_dslr_undistorted/
Workspace=/media/loc/ssd_1tb/multi_view_training_dslr_undistorted/

cd $MODEL_PATH

Scenes="pipes office courtyard delivery_area electro facade kicker meadow playground relief relief_2 terrace terrains"

for scene in $Scenes;
do
   
    DATASET_PATH="$Workspace/$scene"

    echo "PlaneRcnn run :: ${DATASET_PATH}"

    src_dir=$DATASET_PATH/dense/images/dslr_images_undistorted/
    dst_dir=$DATASET_PATH/dense/stereo/plane_segmentation/
    
    echo $src_dir
    
    # clean and creaete dst folder under stereo
    rm -rf $dst_dir
    mkdir -p $dst_dir

    
    # Extract plane 
    python3 simple_inference.py --config=PlaneRecNet_101_config --trained_model=weights/PlaneRecNet_101_9_125000.pth --images=$src_dir:$dst_dir


done 

