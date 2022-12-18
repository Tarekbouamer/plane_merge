#!/bin/bash

Workspace=/media/dl/ssd_1tb/multi_view_training_dslr_undistorted/

Scenes="pipes office courtyard delivery_area electro facade kicker meadow playground relief relief_2 terrace terrains"

for scene in $Scenes;
do
   
    DATASET_PATH="$Workspace/$scene"

    echo "Camera model extractor run :: ${DATASET_PATH}"

    src_dir=$DATASET_PATH/dense/stereo/model

    echo $src_dir
    
    rm -rf $src_dir

    # extract mddel from bin 
    python scripts/database/genFromColmap.py --dataset=$DATASET_PATH


done 

