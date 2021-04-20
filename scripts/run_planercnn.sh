#!/bin/bash

PlaneNetDir=/home/torres/3D/planercnn
Workspace=/home/torres/3D/datasets/eth3d

Scenes="pipes office courtyard delivery_area electro facade kicker meadow playground relief relief_2 terrace terrains"

for scene in $Scenes;
do
   
    DATASET_PATH="$Workspace/$scene"

    echo "PlaneRcnn run :: ${DATASET_PATH}"

    src_dir=$DATASET_PATH/dense/images/dslr_images_undistorted/
    dst_dir=$DATASET_PATH/dense/stereo/plane_segmentation
    model=$DATASET_PATH/dense/stereo/model/

    
    echo $src_dir
    # clean and creaete dst folder under stereo
    rm -rf $dst_dir
    mkdir -p $dst_dir

    
    # Extract plane 
    cd $PlaneNetDir
    python evaluate.py --methods=f --suffix=warping_refine --dataset=inference --images=$src_dir --cameras=$model --output=$dst_dir


done 

