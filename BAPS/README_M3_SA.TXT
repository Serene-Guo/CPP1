BAP README_SA File
-------------------

How to run the BAP program 
--------------------------
  (after it is properly installed and compiled, and leda path properly
  setting)

   
First, go to directory $BAPHOME;
 chmod +x runAll.sh    and then    ./runAll.sh  

 How to compile the BAP program 
------------------------------
  (after it is properly installed)
1. To compile the engine:
   Go to directory $BAPHOME
   type: make
   (if you see any warnings from the linker, it's fine to
    ignore them)

How to install the BAP program
-------------------------------
1. expand the source into directory, say ~
2. Set the following two variables:
   BAPHOME=~/BAPS
   BAPTMP=$BAPHOME/tmp
   
Solution Files
-------------------------------
   Go to directory:  $BAPHOME/solution

Output Files
-------------------------------
    $BAPHOME/out_SA 