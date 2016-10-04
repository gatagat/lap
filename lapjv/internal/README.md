Linear Assignment Problem
-------------------------

R. Jonker and A. Volgenant (University of Amsterdam)
"A Shortest Augmenting Path Algorithm for Dense and Sparse Linear Assignment
 Problems", Computing 38, 325-340 (1987)

This code is not the original version from authors' website. To my knowledge,
there are only two changes:
  1. usage of double precision floating-point numbers for costs, and
  2. a different test for the decrease in the objective that prevents endless
     loop when the optimization gets close to the machine precision (thanks to
     Michael Lewis)
   
The legal status of the code is most probably best described by the few
sentences from the original website:

	These codes are made available in the hope that they will be useful,
	but without any warranty. Also, we cannot accept any responsibility for
	their use.

	The codes are available only for non-commercial use. Please contact
	MagicLogic (http://www.magiclogic.com/) for commercial application.

	All these codes are Copyright © 2003-2008 MagicLogic Systems Inc.,
	Canada.

	For further technical information, please contact A. Volgenant at the
	University of Amsterdam by email.

Tomas Kazmar, 2012
