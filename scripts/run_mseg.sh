
model_name=mseg-3m
model_path=thirdparty/mseg-semantic/weights/mseg-3m.pth
config=thirdparty/mseg-semantic/mseg_semantic/config/test/default_config_360_ms.yaml


Workspace=/media/dl/ssd_1tb/multi_view_training_dslr_undistorted
Weights=thirdparty/MiDaS/weights/dpt_large-midas-2f21e586.pt

Scenes="pipes office courtyard delivery_area electro facade kicker meadow playground relief relief_2 terrace terrains"

for scene in $Scenes;
do
    echo "Mseg run :: ${scene}"

    DATASET_PATH="$Workspace/$scene"

    src_dir=$DATASET_PATH/dense/images/dslr_images_undistorted/
    dst_dir=$DATASET_PATH/dense/stereo/semantic_segmentation
    
    echo $dst_dir
    rm -rf $dst_dir
    mkdir -p $dst_dir
    
    # Extract Semantics
    python thirdparty/mseg-semantic/mseg_semantic/tool/universal_demo.py --config=${config} model_name ${model_name} model_path ${model_path} input_file ${src_dir} save_folder ${dst_dir}

done 
