// setup.sce

//   Set up key variables for control design search.
//
//  version: 27-Sept-2011
//



clear all;
xdel(winsid());   // close all graphics windows which might be open
funcprot(0);      // allow functions to be redefined without warnings.

// correct this for your own computer (may be higher or lower!)
npermin = 549.0;   //  how many step responses per min on this computer

/////////////////////
//  uncomment these lines to make time predictions more accurate

// load('simrate_optigain2','rate');
// npermin = rate;
// printf ("Found rate file: %d simulations per min.\n",npermin);



////////////////////////////////////////////////////////////////
//
//       plant definition
//
s = poly(0,'s');

Htf = s/s;
H = syslin('c', Htf);

tmax = 10;  // seconds
dt = tmax/100;


// Plant Transfer Function

ptf =   3/(s+1)/(s^2 + 0.4*s + 1.04);
plant =      syslin('c', ptf);

////  set this about 10 times higher than highest plant pole/zero
    pp = 10;  //  pole to rationalize PID controller

////////////////////////////   Desired Performance

tsd =   2;   // Desired Settling Time
pod =  1.01;   // Desired Percent Overshoot
ssed =  0.0;   // Desired Steady State Error

cu_max = 200.0;  //  Normalization value for control effort.



////////////////////////////   Search range and step

nvals = 20;  // number of gain values to try btwn kmax and kmin

//  Search region setting:   set K1-K3 for the "center" PID values

// Kp center value
K1 = 0.083 ;

// KI center value
K2 = 0.19;

// Kd center value
K3 = 0.501;


//   "center values" are logarithmic midpoint of search range

scale_range = 5;    // how big a range to search.


tsearch=((nvals+1)^3)/npermin;
if(tsearch < 120) then
   printf("\n\nEstimated search time: %4.1f minutes\n", tsearch);
   else
   printf("\n\nEstimated search time: %4.1f hours\n", tsearch/60.0);
 end

// start the optimzation run automatically
exec('optigain2.sce',-1);


