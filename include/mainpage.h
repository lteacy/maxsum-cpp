/**
 * @file mainpage.h
 * This header defines the main page for this documentation, and is for that
 * purpose only.
 * @author Dr W. T. Luke Teacy, University of Southampton
 */

/**
 * @mainpage
 * \tableofcontents
 * @section intro Overview
 * This library provides an implementation of the max-sum algorithm in C++.
 * For more details about the purpose and theory behind this algorithm, please
 * see http://eprints.soton.ac.uk/265159/
 *
 * To source code for this library can be dowloaded from
 * <a href="https://github.com/lteacy/maxsum-cpp">here</a>.
 *
 * @section usage Library Organisation
 * The contents of this library are organised into two main namespaces:
 * \li ::maxsum, which contains all functions and classes that form part the
 * public iterface to this library; and
 * \li maxsum::util, which contains utility code that forms part of the
 * library implementation.
 *
 * Our intention here is that only functions and types that are of direct
 * interest to third party developers should be located in the ::maxsum
 * namespace. In contrast, those in the maxsum::util interface are not
 * intended to form part of the public interface to the library, and so
 * should only be of interest to developers intended to extend or modify this
 * library.
 *
 * Of the classes in the maxsum namespace, three provide the bulk of the 
 * library's functionality, and should be of particular interest to anyone
 * wishing to apply the max-sum algorithm:
 * \li maxsum::DiscreteFunction, which is used to represent mathematical
 * functions that depend on the cartesian product of a set of variables with
 * finite domains;
 * \li maxsum::DomainIterator, which provides methods for iterating over the
 * domain of maxsum::DiscreteFunction objects; and
 * \li maxsum::MaxSumController, which operates on a \em factor \em graph,
 * to optimise the values assigned to a set of (action) variables.
 *
 * The following sections describe each of these classes in more detail,
 * together with example extracts of code to demonstrate their proper use.
 *
 * @section function_usage The DiscreteFunction Class
 * The maxsum::DiscreteFunction class is the main workhorse of the maxsum
 * library, which not only provides a way to represent mathematical functions,
 * but also to manipulate their values, and combine them using various
 * mathematical operators to form new functions. As the class name suggests,
 * the main limitation is that only functions with a finite domain can be
 * represented, values of which are stored internally for each possible value
 * of a function's domain.
 *
 * In future versions, we may provide other classes which can be used to
 * represent functions with continuous domains. However, for now, the
 * DiscreteFunction class provides all the operations necessary to apply the
 * max-sum algorithm to finite domains.
 * In particular, the following subsections describe the main operations that
 * can be performed using this class.
 *
 * @subsection function_construction Construction
 * The domain of each maxsum::DiscreteFunction is specified by a set of
 * variables, identified by ids of type maxsum::VarID. Each variables, \c k,
 * is associated with a fixed domain size, \f$N_k\f$, which must be registered
 * before the variable is used, and must remain the same through a program's 
 * execution. Once registered, a variable can then take on any integer value
 * in the range \f$[0,N_k]\f$.
 * For example, to create a DiscreteFunction that depends on a single
 * variable, \c 2, we may execute the following code:
 * <pre>
 * maxsum::registerVariable (2, 10);
 * maxsum::DiscreteFunction func(2,3.2);
 * </pre>
 * Here, variable \c 2 is registered with domain size \c 10, which means that
 * it can take on values in the range \f$[0,9]\f$. The 2nd parameter of this
 * constructor is used to initialise all values for this function to a
 * specific value, in this case 3.2. So, for example, we have
 * <pre>
 * num = func.noVars(); // num == 1 (depends on one variable only)
 * siz = func.domainSize(); // siz == 10
 * val = func(0);  // val == 3.2
 * ...
 * val = func(9);  // val == 3.2
 * val = func(10); // result undefined!!
 * </pre>
 * In addition to depending on a single value, DiscreteFunction's can also
 * depend on multiple variables, or the empty set. In the latter, case
 * the function encapsulates a single scalar constant, which does not
 * depend on any variable, and is index by 0:
 * <pre>
 * maxsum::DiscreteFunction func(4.5); // depends on no variables
 * num = func.noVars();     // num==0 (depends on no variables)
 * siz = func.domainSize(); // siz==1 (has only one value)
 * val = func(0); // val == 4.5
 * val = func(1); // result undefined!!
 * </pre>
 * For the former case, there are several options for defining functions
 * that depend on several variables. In particular, we can achieve this
 * by passing a list of variable ids to the constructor using iterators.
 * For example:
 * <pre>
 * maxsum::VarID vars[] = {1,4,8}; // array of size 3
 * maxsum::ValIndex siz[] = {5,10,15}; // var 1 has domain size 5 etc.
 * maxsum::registerVariables (vars,vars+3,siz,size+3);
 * maxsum::DiscreteFunction func(vars,vars+3); // values initialised to 0
 * num = func.noVars(); // num=3 (depends on three variables)
 * siz = func.domainSize(); // siz=5*10*15 (size of cartesian product)
 * val = func(0);   // val == 0
 * val = func(749); // val == 0 (see below for full explanation)
 * val = func(750); // result undefined!!
 * </pre>
 * In addition to these, other constructors include a copy constructor
 * and matching assignment operator; and a default constructor,
 * which creates a function that depends on no variables and has a single value
 * of 0. For details, see the manual page for maxsum::DiscreteFunction.
 *
 * @subsection function_access Element Access
 * As demonstrated in the examples above, the basic and most efficient way
 * to access the values of a function, is to pass a single integer value to the
 * overloaded () operator. This single integer value acts like a linear index
 * for N-D arrays in matlab, except that values start from 0 rather than 1.
 * Moreover, all versions of the () operator return references to values in
 * a DiscreteFunction, and so can be used for setting as well as reading
 * values. For example, the following code is valid.
 * <pre>
 * func(1) = 3.45; // set func(1) to 3.45
 * val = func(1);  // val == 3.45
 * </pre>
 *
 * As in matlab, or indeed any scheme for indexing N-D arrays, these linear
 * indices depend on the order in which each dimension is stored in memory.
 * In our case, each variable in a function's domain corresponds to a dimension
 * in an N-D array, with variables ordered from least to most significant
 * according to their ID. For example, if \c func depends on variables 3 and 5
 * with corresponding domain sizes 2 and 3, then we have the following
 * correspondence between variable values and linear indices:
 * <TABLE>
 * <TR><TD>Variable 3</TD><TD>Variable 5</TD><TD>Linear Index</TD></TR>
 * <TR><TD>0</TD><TD>0</TD><TD>0</TD></TR>
 * <TR><TD>1</TD><TD>0</TD><TD>1</TD></TR>
 * <TR><TD>0</TD><TD>1</TD><TD>2</TD></TR>
 * <TR><TD>1</TD><TD>1</TD><TD>3</TD></TR>
 * <TR><TD>0</TD><TD>2</TD><TD>4</TD></TR>
 * <TR><TD>1</TD><TD>2</TD><TD>5</TD></TR>
 * </TABLE>
 * Here, notice how variable 3, having the smalled variable id, is incremented
 * first, before the variable with the next largest id, which is 5. The
 * situation is the same for variables with more than two variables: the
 * variable with the smallest id is incremented first, and so on until the
 * variable with the highest id, which is incremented last.
 *
 * Although the most efficient way to access elements using linear indices,
 * these are often not available, and so must be calculated from a set of
 * subindices, relating to named variables. For this reason, several other
 * versions of the () operator are available that except subindices.
 * For example, if variable names and values are stored in lists, s.t.
 * the kth index gives the value for the kth variable, then we can access
 * elements as follows.
 * <pre>
 * maxsum::VarID vars[] = {1,4,8}; // array of size 3
 * maxsum::ValIndex siz[] = {5,10,15}; // var 1 has domain size 5 etc.
 * maxsum::registerVariables (vars,vars+3,siz,size+3);
 * maxsum::DiscreteFunction func(vars,vars+3); // values initialised to 0
 *
 *  std::vector<VarID> varVec(vars,vars+3); // copy ids into vector
 * std::vector<ValIndex> valVec(3); // vector of size 3
 * valVec[0] = 2; // index value for variable 1
 * valVec[1] = 4; // index value for variable 4
 * valVec[2] = 5; // index value for variable 8
 *
 *  // THE FOLLOWING STATEMENTS ALL ACCESS THE SAME ELEMENT
 * int linearIndex = 2+4*10+5*15*10; // corresponding linear index
 * func(2,4,5); // specify indices in order of variable id
 * func(valVec.begin(),valVec.end()); // indices in order of variable id
 * func(linearIndex); // access by corresponding linear index
 * func(varVec,valVec); // access by named variables in lists (does not work for arrays)
 * func(vars,vars+3,valVec.begin(),valVec.end()); // access by named variables using iterators
 * func(varVec.begin(),varVec.end(),valVec.begin(),valVec.end()); 
 * </pre>
 * Here, the last three variants all work by specifying named variables and
 * corresponding values in lists, either by passing reference to containers
 * directly (which must supply interfaces consistent with standard library
 * containers), or by passing iterators to such lists. In each case, the kth
 * value is assumed to correspond to the kth named variable. Any values
 * specified for variables that are not in the functions domain are ignored.
 *
 * For some versions of the () operator, the member function
 * maxsum::DiscreteFunction::at is also provided for convenient element access
 * through pointers. For example, the following code is valid:
 * <pre>
 * maxsum::registerVariable (3,5);
 * maxsum::DiscreteFunction func(3,4.5);
 * maxsum::DiscreteFunction* pFunc = &func;
 * val = (*pFunc)(2);  // works, but ugly
 * val = pFunc->at(2); // equivalent, but more readable
 * </pre>
 *
 * @subsection math_ops Mathematical Operations
 * Three main types of mathematical operation are provided by the
 * maxsum::DiscreteFunction class:
 * \li \em scalar operations, which return a single scalar value based on all
 * values for a maxsum::DiscreteFunction across its domain;
 * \li \em domain operations, which expand or reduce the domain of a single
 * function; and
 * \li \em arithmetic operations, which combine one or more functions to
 * form a new maxsum::DiscreteFunction.
 * 
 * We describe each of these in turn below.
 * @subsubsection scalar_ops Scalar Operations
 * todo
 *
 * @subsubsection domain_ops Domain Operations
 *
 * @subsubsection arithmetic_ops Arithmetic Operations
 *
 * @subsection comparision_ops Comparison Operations
 *
 * @subsection misc_ops Miscellaneous Operations
 *
 * @section domainIterator The DomainIterator Class
 * Todo.
 *
 * @section maxsumcontroller The MaxSumController Class
 * Todo.

 * @section tips Tips on Writing Efficient Code
 * Todo.
 *
 * @section futurework Future Work
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
