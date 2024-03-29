Workspace=/media/dl/Data/datasets/multi_view_training_dslr_undistorted/

Scenes="pipes office courtyard delivery_area electro facade kicker meadow playground relief relief_2 terrace terrains"

for scene in $Scenes;
do
    echo "Colmap run :: ${scene}"
    DATASET_PATH="$Workspace/$scene"

    echo "Colmap run :: ${DATASET_PATH}"
    rm -rf $DATASET_PATH/sparse
    rm -rf $DATASET_PATH/dense
    
    # Feature extraction 
    colmap feature_extractor --database_path $DATASET_PATH/database.db --image_path $DATASET_PATH/images
    colmap exhaustive_matcher --database_path $DATASET_PATH/database.db
    
    # Sparse 
    mkdir -p $DATASET_PATH/sparse
    colmap hierarchical_mapper --database_path $DATASET_PATH/database.db --image_path $DATASET_PATH/images --output_path $DATASET_PATH/sparse

    # Dense 
    mkdir -p $DATASET_PATH/dense
    colmap image_undistorter --image_path $DATASET_PATH/images --input_path $DATASET_PATH/sparse/0 --output_path $DATASET_PATH/dense --output_type COLMAP --max_image_size 1024
    colmap patch_match_stereo --workspace_path $DATASET_PATH/dense --workspace_format COLMAP --PatchMatchStereo.geom_consistency true

    # Fusion 
    # colmap stereo_fusion --workspace_path $DATASET_PATH/dense --workspace_format COLMAP --input_type geometric --output_path $DATASET_PATH/dense/fused.ply

    # # Meshing 
    # colmap poisson_mesher --input_path $DATASET_PATH/dense/fused.ply --output_path $DATASET_PATH/dense/meshed-poisson.ply
    # colmap delaunay_mesher --input_path $DATASET_PATH/dense --output_path $DATASET_PATH/dense/meshed-delaunay.ply
done 
