
model_name=mseg-3m
model_path=./logs/mseg-3m.pth
config=mseg_semantic/config/test/default_config_360_ms.yaml


MsegNetDir=/home/torres/3D/mseg-semantic

Workspace=/home/torres/3D/datasets/eth3d

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
    cd $MsegNetDir
    python -u scripts/mseg_semantic/tool/universal_demo.py --config=${config} model_name ${model_name} model_path ${model_path} input_file ${src_dir} save_folder ${dst_dir}

done 
