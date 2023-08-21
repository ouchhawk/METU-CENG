module Hw2 where

data HuffmanTree = Empty | Intermediate Integer HuffmanTree HuffmanTree | Leaf Char Integer deriving (Show,Eq)


frequencies :: [Char] -> [HuffmanTree]

buildTree :: [HuffmanTree] -> HuffmanTree

encode :: HuffmanTree -> [Char] -> [Char]

decode :: HuffmanTree -> [Char] -> [Char]

--DO NOT MODIFY ABOVE THIS LINE
--Start here
----------------------------------------------------------------------------------------------------------anlamli part
ap :: HuffmanTree -> [Char] -> [Char]
ap (Intermediate i h1 h2) (x:xs) = if (x=='0' && isleaf(h1)==True) then ['0']
				else if (x=='0' && isleaf(h1)==False) then (ap (h1) xs) ++['0']
				else if (x=='1' && isleaf(h2)==True) then ['1']
				else if (x=='1' && isleaf(h2)==False) then (ap (h2) xs) ++['1']
				else []
ap (Leaf c i) (x:xs) = [x]
ap _ [] = []
----------------------------------------------------------------------------------------------------------encode2
encode2 Empty _ = []
encode2 _ [] = []
encode2 (Leaf c i) _ = []
encode2 (Intermediate i h1 h2) (x:xs) = if (ischild h1 x)==True then (['0'] ++ (encode(h1) (x:xs)))
					else (['1'] ++ (encode(h2) (x:xs)))
----------------------------------------------------------------------------------------------------------decode2
decode2 Empty _ = []
decode2 _ [] = []
decode2 (Leaf c i) _ = [c]
decode2 (Intermediate i h1 h2) (x:xs) = if (x=='0') && (isleaf h1)==True then (decode2 h1 (x:xs))
					else if (x=='0') && (isleaf h1)==False then (decode2 h1 xs)
					else if (x=='1') && (isleaf h2)==True then (decode2 h2 (x:xs))
					else if (x=='1') && (isleaf h2)==False then (decode2 h2 xs)
					else []
----------------------------------------------------------------------------------------------------------ischild ?
ischild :: HuffmanTree -> Char -> Bool
ischild (Leaf c1 i1) x = if (c1==x) then True
					else False
ischild (Intermediate i (Intermediate i1 h1 h2) (Intermediate i2 h3 h4)) x = if ((ischild(Intermediate i1 h1 h2) x)||(ischild(Intermediate i2 h3 h4) x))==True then True
				  			else False
ischild (Intermediate i (Leaf c1 i1) (Intermediate i2 h1 h2)) x = if ((ischild(Leaf c1 i1) x)||(ischild(Intermediate i2 h1 h2) x))==True then True
				  			else False 
ischild (Intermediate i (Intermediate i2 h1 h2) (Leaf c1 i1)) x = if ((ischild(Leaf c1 i1) x)||(ischild(Intermediate i2 h1 h2) x))==True then True
				  			else False 
ischild (Intermediate i (Leaf c1 i1) (Leaf c2 i2)) x = if ((ischild(Leaf c1 i1) x)||(ischild(Leaf c2 i2) x))==True then True
				  			else False 
----------------------------------------------------------------------------------------------------------isleaf ?
isleaf :: HuffmanTree -> Bool
isleaf (Leaf c i) = True
isleaf _ = False
----------------------------------------------------------------------------------------------------------count first
countfst :: [Char] -> Integer
countfst (x:xs) = toInteger(length(filter (==x) (x:xs)))
----------------------------------------------------------------------------------------------------------ordering
order :: [HuffmanTree] -> [HuffmanTree]
order ((Leaf c1 i1):((Leaf c2 i2):x)) = if (i1>i2) || ((i1==i2) && (c1>c2)) then ((Leaf c2 i2):order((Leaf c1 i1):x))
					else ((Leaf c1 i1):order((Leaf c2 i2):x))
order ((Intermediate i1 h1 h2):((Leaf c1 i2):x)) = if (i1>i2) || (i1==i2) then ((Leaf c1 i2):order((Intermediate i1 h1 h2):x))
					else ((Intermediate i1 h1 h2):order((Leaf c1 i2):x))
order ((Leaf c1 i1):((Intermediate i2 h1 h2):x)) = if (i1>i2) then ((Intermediate i2 h1 h2):order((Leaf c1 i1):x))
					else ((Leaf c1 i1):order((Intermediate i2 h1 h2):x))
order ((Intermediate i1 h1 h2):((Intermediate i2 h3 h4):x)) = if (i1>i2) then ((Intermediate i2 h3 h4):order((Intermediate i1 h1 h2):x))
					else ((Intermediate i1 h1 h2):order((Intermediate i2 h3 h4):x))
order a = a    --altta kalcak (dokunma)
----------------------------------------------------------------------------------------------------------intermediate creater
inter :: [HuffmanTree] -> HuffmanTree
inter ((Leaf c1 i1):((Leaf c2 i2):x)) = (Intermediate (i1+i2) (Leaf c1 i1) (Leaf c2 i2))
inter ((Intermediate i1 h1 h2):((Leaf c1 i2):x)) = (Intermediate (i1+i2) (Intermediate i1 h1 h2) (Leaf c1 i2))
inter ((Leaf c1 i1):((Intermediate i2 h1 h2):x)) = (Intermediate (i1+i2) (Leaf c1 i1) (Intermediate i2 h1 h2))
inter ((Intermediate i1 h1 h2):((Intermediate i2 h3 h4):x)) = (Intermediate (i1+i2) (Intermediate i1 h1 h2) (Intermediate i2 h3 h4))
----------------------------------------------------------------------------------------------------------FREQUENCIES
frequencies [] = []
frequencies (x:xs) = if length(x:xs)==1 then [Leaf x 0]
			else order([Leaf x (countfst (x:xs))]++(filter (/= Empty) (frequencies (filter(/=x) xs))))
----------------------------------------------------------------------------------------------------------BUILDTREE
buildTree [] = Empty
buildTree [Intermediate a h1 h2] = Intermediate a h1 h2
buildTree (a:(b:x)) = buildTree (order (filter(/=Empty) ([inter(a:(b:x))]++(x))))
----------------------------------------------------------------------------------------------------------ENCODE
encode Empty _ = []
encode _ [] = []
encode (Leaf c i) _ = []
encode (Intermediate i h1 h2) (x:xs) = if (ischild h1 x)==True then (['0'] ++ (encode2(h1) (x:xs))) ++ (encode (Intermediate i h1 h2) (xs))
								else (['1'] ++ (encode2(h2) (x:xs))) ++ (encode (Intermediate i h1 h2) (xs))
----------------------------------------------------------------------------------------------------------DECODE
decode Empty _ = []
decode _ [] = []
decode (Leaf c i) _ = [c]
decode (Intermediate i h1 h2) (x:xs) = if ((x=='0') && (isleaf h1)) == True then (decode2 h1 (x:xs))++(decode (Intermediate i h1 h2) xs)
				else if ((x=='0') && (isleaf h1) == False) then (decode2 h1 xs)++(decode (Intermediate i h1 h2) (drop (length(ap (h2) (x:xs))) xs))
				else if ((x=='1') && (isleaf h2)) == True then (decode2 h2 (x:xs))++(decode (Intermediate i h1 h2) xs)
				else if ((x=='1') && (isleaf h2) == False) then (decode2 h2 xs)++(decode (Intermediate i h1 h2) (drop (length(ap (h2) (x:xs))) xs))
				else []
