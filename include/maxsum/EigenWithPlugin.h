/**
 * Wrapper for Eigen3 library, which provides standard library style iterators
 * for eigen library arrays and matrices.
 */
#ifndef EIGEN_WITH_PLUGIN_H
#define EIGEN_WITH_PLUGIN_H

// we want to use eigen within multiple threads, so best not let eigen
// parallelise itself.
#define EIGEN_DONT_PARALLELIZE 

#ifdef EIGEN_DENSEBASE_H // don't let Eigen compile without plugin
YOU_MUST_INCLUDE_PLUGIN_BEFORE_EIGEN_HEADERS
#endif

#include "EigenIterator.h"
#define EIGEN_DENSEBASE_PLUGIN "maxsum/EigenIteratorPlugin.h"
#define EIGEN_MATRIXBASE_PLUGIN "maxsum/MatrixBaseAddons.h"
#include <Eigen/Dense>

#endif // EIGEN_WITH_PLUGIN_H
