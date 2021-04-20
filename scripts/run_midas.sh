
model_name=mseg-3m
model_path=./logs/mseg-3m.pth
config=mseg_semantic/config/test/default_config_360_ms.yaml


MidasNetDir=/home/torres/3D/MiDaS
Workspace=/home/torres/3D/datasets/eth3d

Scenes="courtyard delivery_area electro facade kicker meadow office pipes playground relief relief_2 terrace terrains"

for scene in $Scenes;
do
    echo "Midas run :: ${scene}"

    DATASET_PATH="$Workspace/$scene"

    src_dir=$DATASET_PATH/dense/images/dslr_images_undistorted/
    dst_dir=$DATASET_PATH/dense/stereo/mono_depth
    

    echo $dst_dir
    rm -rf $dst_dir
    mkdir -p $dst_dir
    
    # Estimate mono depth
    cd $MidasNetDir
    python run.py -i $src_dir -o $dst_dir
done 
