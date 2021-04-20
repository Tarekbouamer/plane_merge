#!/bin/bash


PlaneNetDir=/home/torres/3D/planercnn
Workspace=/home/torres/3D/datasets/eth3d

Scenes="pipes office courtyard delivery_area electro facade kicker meadow playground relief relief_2 terrace terrains"

for scene in $Scenes;
do
   
    DATASET_PATH="$Workspace/$scene"

    echo "Database extractor run :: ${DATASET_PATH}"

    src_dir=$DATASET_PATH/dense/stereo/model

    echo $src_dir
    
    rm -rf $src_dir

    # Extract mddel from bin 
    python database/genFromColmap.py --dataset=$DATASET_PATH


done 

