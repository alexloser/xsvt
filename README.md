# XSVT
Small tools for csv file processing (onehot encoding, format  checking and converting to libsvm).

XSV means the delimeter could be comma("," csv), tab("\t", tsv) and space(" ")

Contains three parts: 
 * Format checker  
 * Onehot encoder  
 * Transfer to libsvm format convert  
 
In fact, the core function is onehot encoding for machine-learning.

## Dependency:
No dependency, pure c code.

## Example:

#### Before onehot encoding:  
![Image text](https://github.com/alexloser/xsvt/blob/master/before.png)  

#### After onehot encoding:  
![Image text](https://github.com/alexloser/xsvt/blob/master/after.png)  

### Notice:
 * Onehot only process non-numeric values.  
 * The matrix(csv) after encoded maybe very large because more 0-1 values expanded!  
   In fact it's sparse matrix, so libsvm format is another choice.

 
## Build and Test:
> make clean  
> make -j2  
> cd test  
> ./test-checker.sh  
> ./test-onehot.sh  
> ./test-transfer.sh  

## Usage:
 * Format checker
```
Usage: ./xsvt.checker [OPTIONS]  
  -i : input csv file to check.  
  -d : delimiter of csv file, default is comma.  
  -h : show this help.    
```

 * OneHot encoder  
```
Usage: ./xsvt.onehot [OPTIONS]
  -h : Show this help
  -i : Input filename(.csv) to encode
  -o : Output filename(.csv) to save results
  -d : Delimiter of csv file
  -k : Has csv header at first line, 1 or 0, default is 1(has)
  -w : Write header in output file, 1 or 0, default is 1(write)
  -c : Check csv format, only check, no encoding work!
Example:
    ./xsvt.onehot -i input.csv -c
    ./xsvt.onehot -i input.csv -o out.csv -d "," -w 1 -k 1
    ./xsvt.onehot -i input.csv -o out.csv -w 1 -k 0
Notice:
    Using -c option to check the format before encoding is a good idea!!!
```

 * Libsvm transfer
```
Usage: ./xsvt.transfer [OPTIONS]
  -h : Show this help.
  -i : Input filename(csv or svm) to convert.
  -o : Output filename to save csv or svm format.
  -l : label filename for read(when csv=>svm) or write(when svm=>csv).
  -d : Delimiter of csv file, default is comma.
  -k : Has csv header at first line, 1 or 0, default is 1(has).
  -s2c : SVM => CSV.
  -c2s : CSV => SVM.
Example: convert in.csv to out.svm
  ./xsvt.transfer -c2s -i in.csv -l in_label.csv -o out.svm -d "," -k 1
Example: convert in.svm to out.csv
  ./xsvt.transfer -s2c -i in.svm -o out.csv -l out_label.csv
```


 