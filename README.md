# U06_PL_RadixSort_TBB
El objetivo de esta practica es implementar el algoritmo Radix sort utilizando la librería Intel TBB para ordenar un vector de numero.
Para realizar el ejercicio he utilizado de referencia el ejercicio 6_Example_PackingProblem.

Procedimiento:
  1. Hemos generado dos arrays, una para poder obtener que posiciones del vector a ordenar tienen como valor un numero cuyo último bit
      vale 1 y otra para poder obtener que posiciones del vector cuyo bit valga cero.
      
  2. Realizamos una acumulación de los 1s de ambas arrays y calculamos el número total
  
  3. A partir de estas acumulaciones realizamos la ordenación del vector de números teniendo en este bit
  
  4. Realizamos los pasos anteriores para cada uno de los bits para ello hacemos uso de una máscara que vamos incrementando.
  
