#!/bin/bash

Workspace=/media/dl/Data/datasets/multi_view_training_dslr_undistorted/

Scenes="pipes office courtyard delivery_area electro facade kicker meadow playground relief relief_2 terrace terrains"

cd build 

for scene in $Scenes;
do
   
    PROJECT_PATH="$Workspace/$scene"

    echo "Simple Merge for :: ${PROJECT_PATH}"
    
    # extract mddel from bin 
    ./SceneParser simple_merge --project_path $PROJECT_PATH

done 
