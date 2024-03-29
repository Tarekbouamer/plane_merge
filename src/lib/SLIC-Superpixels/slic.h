#ifndef SLIC_H
#define SLIC_H

/* slic.h.
 *
 * Written by: Pascal Mettes.
 *
 * This file contains the class elements of the class Slic. This class is an
 * implementation of the SLIC Superpixel algorithm by Achanta et al. [PAMI'12,
 * vol. 34, num. 11, pp. 2274-2282].
 *
 * This implementation is created for the specific purpose of creating
 * over-segmentations in an OpenCV-based environment.
 */

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <math.h>
#include <vector>
#include <float.h>
using namespace std;

/* 2d matrices are handled by 2d vectors. */
#define vec2dd vector<vector<double> >
#define vec2di vector<vector<int> >
#define vec2db vector<vector<bool> >
/* The number of iterations run by the clustering algorithm. */
#define NR_ITERATIONS 10

/*
 * class Slic.
 *
 * In this class, an over-segmentation is created of an image, provided by the
 * step-size (distance between initial cluster locations) and the colour
 * distance parameter.
 */
class Slic {
    private:
        /* The cluster assignments and distance values for each pixel. */
		vec2di  clusters;
        vec2dd distances;
        
        /* The LAB and xy values of the centers. */
        vec2dd centers;

        /* The number of occurences of each center. */
        vector<int> center_counts;
        
        /* The step size per cluster, and the colour (nc) and distance (ns)
         * parameters. */
        int step, nc, ns;
        
        /* Compute the distance between a center and an individual pixel. */
        double compute_dist(int ci, cv::Point pixel, cv::Vec3b colour);

        /* Find the pixel with the lowest gradient in a 3x3 surrounding. */
        cv::Point find_local_minimum(cv::Mat image, cv::Point center);
        
        /* Remove and initialize the 2d vectors. */
        void clear_data();
        void init_data(cv::Mat image);

    public:
        /* Class constructors and deconstructors. */
        Slic();
        ~Slic();
        
        /* Generate an over-segmentation for an image. */
        void generate_superpixels(cv::Mat image, int step, int nc);

        /* Enforce connectivity for an image. */
        void create_connectivity( cv::Mat image);
        
        /* Draw functions. Resp. displayal of the centers and the contours. */
        void display_center_grid(cv::Mat image, cv::Scalar colour);
        void display_contours(cv::Mat image, cv::Vec3b colour);
        void colour_with_cluster_means(cv::Mat image);

        void log();
};

#endif
