# ABY
## Topic
- Implementation and Performance Optimization of C Standard Library for Secure Computation between Two Parties.
### skill
- Cmake, C++
#### using tool 
- Vtune, gprof , Vscode
## implementation flow
![Alt text](image-2.png)
## Current Progress
1. support for C standard library in the MPC version
    
---
( 1 ) basic function
||CPU time(s)|support data type |Finish(%) |Verification|
|-----|--------|-----|--------|--|
|Floor|0.267s|double|80%||
|Ceil|0.275s|double|80%||
|Fabs|0.366s|double|80%||
|Power|2.111s|double|80%||
|exp|0.488s|float|80%||
|sqrt|0.720s|double|80%||

---
( 2 ) Trigonometric function
||CPU time(s)|support data type |Finish(%) |Verification|
|-----|--------|-----|--------|--|
|sin|0.395s|double|80%||
|cos|0.394s|double|80%||
|tan|0.432s|double|80%||
|asin|29.755s|double|80%||
|acos|29.187s|double|80%||
|atan|40.844s|double|80%||
|sinh|1.285s|double|80%||
|cosh|1.243s|double|80%||
|tanh|1.092s|double|80%||

---
( 3 ) Modulus operation function
||CPU time(s)|support data type |Finish(%) |Verification|
|-----|--------|-----|--------|--|
|fmod|0.842s|double|80%||
|modf|0.366s|double|80%||

---
2. verification 
    - Currently, the validation method involves comparing the results with those 
      obtained from the original C standard library functions.
    - Test data is approximately 10 records and has not been extensively tested yet.
---
3. Optimization has been completed using Intel-supported intrinsic functions, 
  resulting in improved program performance as follows.
    - website
    - https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
---

![Alt text](image.png)
![Alt text](image-1.png)

---
## Expected Future Progress
1. Completeness of the verification method
2. Integrating existing functions to provide users with more convenience in usage
 

