/**
 * @file mainpage.h
 * This header defines the main page for this documentation, and is for that
 * purpose only.
 * @author Dr W. T. Luke Teacy, University of Southampton
 */

/**
 * @mainpage
 * @section intro Overview
 * This library provides an implementation of the max-sum algorithm in C++.
 * For more details about the purpose and theory behind this algorithm, please
 * see http://eprints.soton.ac.uk/265159/
 *
 * @section Basic Usage
 * There are two main classes that define the interface to this library:
 * \li maxsum::DiscreteFunction, which is used to represent mathematical
 * functions that depend on the cartesian product of a set of variables with
 * finite domains; and
 * \li maxsum::MaxSumController, which operates on a \em factor \em graph,
 * to optimise the values assigned to a set of (action) variables.
 *
 * @section Future Work
 * Although the basic implementation of this library is now complete, this
 * project is still under going its first phase of active development. 
 * Accurate results are thus not yet guarranteed, and in fact, bugs are
 * very much still to be expected.
 *
 * For now, our main priority is thus continued testing and debugging.
 * Beyond that, we also plan to look at various options for optimising the
 * code. For example, one option is to link to the
 * <a href="http://eigen.tuxfamily.org">eigen3</a> linear algebra library,
 * to take advantage of its hardware vectorisation.
 *
 * We will also continue to improve this documentation of the API.
 * 
 */
