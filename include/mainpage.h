/**
 * @file mainpage.h
 * This header defines the main page for this documentation, and is for that
 * purpose only.
 * @author Dr W. T. Luke Teacy, University of Southampton
 */

/**
 * @mainpage
 * This is the main page
 * @section intro Overview
 * This library defines some stuff.
 * @section future Future Work
 * As future work we plan to optimise this library by using the vectorisation
 * functionality of the eigen3 library. In particular, if we analysis the
 * domain intersection for two or more maxsum::DiscreteFunction objects, we
 * should be able to identify subblocks of values that can be operated on
 * in parallel. Replicating vectors column-wise or row-wise might be one way
 * to achieve this. Think about order of variables - if domain of one operand
 * is a continuous subset of the other, then iterating over continuous blocks
 * rather than individual elements might be possible. However, we should keep
 * non-vectorised code for comparision, and as conditional compilation option
 * when the eigen library is not available.
 * @todo write main page documentation
 * @todo vectorise library using eigen3.
 */
