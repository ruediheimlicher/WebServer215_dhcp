FasdUAS 1.101.10   ��   ��    k             l   � ����  O    �  	  k   � 
 
     I   	������
�� .miscactvnull��� ��� obj ��  ��        l  
 
��������  ��  ��        r   
     m   
    �      o      ���� 0 filecontents fileContents      r        4    �� 
�� 
alis  l    ����  l    ����  I   ��  
�� .earsffdralis        afdr   f      �� ��
�� 
rtyp  m    ��
�� 
ctxt��  ��  ��  ��  ��    o      ���� 0 
homeordner         l   �� ! "��   ! 0 *display dialog "homeordner: " & homeordner    " � # # T d i s p l a y   d i a l o g   " h o m e o r d n e r :   "   &   h o m e o r d n e r    $ % $ r      & ' & n     ( ) ( m    ��
�� 
ctnr ) o    ���� 0 
homeordner   ' o      ���� 0 homeordnerpfad   %  * + * l  ! !�� , -��   ,  open homeordnerpfad    - � . . & o p e n   h o m e o r d n e r p f a d +  / 0 / l  ! !�� 1 2��   1 8 2display dialog "homeordnerpfad: " & homeordnerpfad    2 � 3 3 d d i s p l a y   d i a l o g   " h o m e o r d n e r p f a d :   "   &   h o m e o r d n e r p f a d 0  4 5 4 l  ! !�� 6 7��   6 2 ,set main to file "datum.c" of homeordnerpfad    7 � 8 8 X s e t   m a i n   t o   f i l e   " d a t u m . c "   o f   h o m e o r d n e r p f a d 5  9 : 9 r   ! ( ; < ; b   ! & = > = l  ! $ ?���� ? c   ! $ @ A @ o   ! "���� 0 homeordnerpfad   A m   " #��
�� 
TEXT��  ��   > m   $ % B B � C C  L o g f i l e . t x t < o      ���� 0 filepfad   :  D E D I  ) 0�� F��
�� .sysodlogaskr        TEXT F b   ) , G H G m   ) * I I � J J  f i l e p f a d :   H o   * +���� 0 filepfad  ��   E  K L K l  1 1�� M N��   M ! tell application "TextEdit"    N � O O 6 t e l l   a p p l i c a t i o n   " T e x t E d i t " L  P Q P I  1 6������
�� .miscactvnull��� ��� obj ��  ��   Q  R S R r   7 I T U T l  7 E V���� V I  7 E�� W X
�� .rdwropenshor       file W 4   7 =�� Y
�� 
file Y o   ; <���� 0 filepfad   X �� Z��
�� 
perm Z m   @ A��
�� boovtrue��  ��  ��   U o      ���� 0 refnum RefNum S  [ \ [ Q   J� ] ^ _ ] k   M} ` `  a b a r   M V c d c l  M T e���� e I  M T�� f��
�� .rdwrread****        **** f o   M P���� 0 refnum RefNum��  ��  ��   d o      ���� 0 filecontents fileContents b  g h g l  W W��������  ��  ��   h  i j i l  W W�� k l��   k 7 1display dialog "inhalt: " & return & fileContents    l � m m b d i s p l a y   d i a l o g   " i n h a l t :   "   &   r e t u r n   &   f i l e C o n t e n t s j  n o n l  W W�� p q��   p . (set Datum to paragraph 2 of fileContents    q � r r P s e t   D a t u m   t o   p a r a g r a p h   2   o f   f i l e C o n t e n t s o  s t s l  W W�� u v��   u &  display dialog "Datum: " & Datum    v � w w @ d i s p l a y   d i a l o g   " D a t u m :   "   &   D a t u m t  x y x r   W ` z { z I  W \������
�� .misccurdldt    ��� null��  ��   { o      ���� 	0 heute   y  | } | l  a a�� ~ ��   ~ &  display dialog "heute: " & heute     � � � @ d i s p l a y   d i a l o g   " h e u t e :   "   &   h e u t e }  � � � r   a l � � � n   a h � � � 1   d h��
�� 
year � o   a d���� 	0 heute   � o      ���� 0 jahrtext   �  � � � r   m x � � � n   m t � � � m   p t��
�� 
mnth � o   m p���� 	0 heute   � o      ���� 0 	monattext   �  � � � l  y y�� � ���   � * $display dialog "monat: " & monattext    � � � � H d i s p l a y   d i a l o g   " m o n a t :   "   &   m o n a t t e x t �  � � � r   y � � � � n   y � � � � 7  � ��� � �
�� 
ctxt � m   � ������� � m   � ������� � l  y � ����� � b   y � � � � m   y | � � � � �  0 � n   | � � � � 1    ���
�� 
day  � o   | ���� 	0 heute  ��  ��   � o      ���� 0 tag   �  � � � l  � ��� � ���   � " display dialog "tag: " & tag    � � � � 8 d i s p l a y   d i a l o g   " t a g :   "   &   t a g �  � � � r   � � � � � J   � � � �  � � � m   � ���
�� 
jan  �  � � � m   � ���
�� 
feb  �  � � � m   � ���
�� 
mar  �  � � � l 	 � � ����� � m   � ���
�� 
apr ��  ��   �  � � � m   � ���
�� 
may  �  � � � m   � ���
�� 
jun  �  � � � m   � ���
�� 
jul  �  � � � m   � ���
�� 
aug  �  � � � l 	 � � ����� � m   � ���
�� 
sep ��  ��   �  � � � m   � ���
�� 
oct  �  � � � m   � ���
�� 
nov  �  ��� � m   � ���
�� 
dec ��   � o      ���� 0 monatsliste MonatsListe �  � � � Y   � � ��� � ��� � Z   � � � ����� � =   � � � � � o   � ����� 0 	monattext   � n   � � � � � 4   � ��� �
�� 
cobj � o   � ����� 0 i   � o   � ����� 0 monatsliste MonatsListe � k   � � � �  � � � r   � � � � � n   � � � � � 7  � ��� � �
�� 
ctxt � m   � ������� � m   � ������� � l  � � ����� � b   � � � � � m   � � � � � � �  0 � o   � ����� 0 i  ��  ��   � o      ���� 	0 monat   �  ��� � l  � � � � � �  S   � � � - ' wenn true, wird die Schleife verlassen    � � � � N   w e n n   t r u e ,   w i r d   d i e   S c h l e i f e   v e r l a s s e n��  ��  ��  �� 0 i   � m   � �����  � m   � ����� ��   �  � � � l  � ��� � ���   � &  display dialog "monat: " & monat    � � � � @ d i s p l a y   d i a l o g   " m o n a t :   "   &   m o n a t �  � � � r   � � � � l 	 � ����� � l  � ����� � n  � � � � 7 �� � �
�� 
cha  � m  	����  � m  ����  � l  � ����� � c   � � � � o   ����� 0 jahrtext   � m  �
� 
ctxt��  ��  ��  ��  ��  ��   � o      �~�~ 0 jahr   �  � � � l �} � ��}   � ? 9display dialog "jahr: " & jahr & " jahrtext: " & jahrtext    � � � � r d i s p l a y   d i a l o g   " j a h r :   "   &   j a h r   &   "   j a h r t e x t :   "   &   j a h r t e x t �  � � � l �| � ��|   � , &set l to number of characters of Datum    � � � � L s e t   l   t o   n u m b e r   o f   c h a r a c t e r s   o f   D a t u m �  �  � l �{�{   1 +set neuesDatum to text -l thru -13 of Datum    � V s e t   n e u e s D a t u m   t o   t e x t   - l   t h r u   - 1 3   o f   D a t u m   l �z�z   P Jset neuesDatum to text 1 thru 14 of Datum -- Anfang bis und mit Leerschlag    � � s e t   n e u e s D a t u m   t o   t e x t   1   t h r u   1 4   o f   D a t u m   - -   A n f a n g   b i s   u n d   m i t   L e e r s c h l a g 	
	 r  - b  ) b  % b  ! b   o  �y�y 0 tag   m   �  . o   �x�x 	0 monat   m  !$ �  . o  %(�w�w 0 jahrtext   o      �v�v 0 
neuesdatum 
neuesDatum
  l ..�u�u   0 *display dialog "neuesDatum: " & neuesDatum    � T d i s p l a y   d i a l o g   " n e u e s D a t u m :   "   &   n e u e s D a t u m  r  .[ !  b  .W"#" b  .S$%$ b  .Q&'& b  .M()( b  .I*+* b  .E,-, b  .A./. b  .=010 b  .9232 b  .5454 m  .166 �77 T * * * * * * * * * * * * * * * * * * * * * *                                        5 o  14�t�t 0 
neuesdatum 
neuesDatum3 o  58�s
�s 
ret 1 l 	9<8�r�q8 o  9<�p
�p 
ret �r  �q  / o  =@�o
�o 
ret - o  AD�n
�n 
ret + o  EH�m
�m 
ret ) l 	IL9�l�k9 m  IL:: �;; , * * * * * * * * * * * * * * * * * * * * * *�l  �k  ' o  MP�j
�j 
ret % o  QR�i�i 0 filecontents fileContents# o  SV�h
�h 
ret ! o      �g�g 0 	neuertext 	neuerText <=< I \g�f>?
�f .rdwrseofnull���     ****> o  \_�e�e 0 refnum RefNum? �d@�c
�d 
set2@ m  bc�b�b  �c  = ABA I hu�aCD
�a .rdwrwritnull���     ****C o  hk�`�` 0 	neuertext 	neuerTextD �_E�^
�_ 
refnE o  nq�]�] 0 refnum RefNum�^  B F�\F I v}�[G�Z
�[ .rdwrclosnull���     ****G o  vy�Y�Y 0 refnum RefNum�Z  �\   ^ R      �X�W�V
�X .ascrerr ****      � ****�W  �V   _ k  ��HH IJI I ���UK�T
�U .sysodlogaskr        TEXTK m  ��LL �MM  k e i n   L o g�T  J N�SN I ���RO�Q
�R .rdwrclosnull���     ****O o  ���P�P 0 refnum RefNum�Q  �S   \ PQP l ���ORS�O  R  start   S �TT 
 s t a r tQ UVU r  ��WXW J  ��YY Z�NZ m  ��[[ �\\  x c o d e p r o j�N  X o      �M�M 0 filetype  V ]^] l ���L_`�L  _ ? 9set projektpfad to (path to alias (homeordner)) as string   ` �aa r s e t   p r o j e k t p f a d   t o   ( p a t h   t o   a l i a s   ( h o m e o r d n e r ) )   a s   s t r i n g^ bcb l ���Kde�K  d 0 *display dialog "projektpfad" & projektpfad   e �ff T d i s p l a y   d i a l o g   " p r o j e k t p f a d "   &   p r o j e k t p f a dc ghg l ���Jij�J  i 8 2display dialog "homeordnerpfad: " & homeordnerpfad   j �kk d d i s p l a y   d i a l o g   " h o m e o r d n e r p f a d :   "   &   h o m e o r d n e r p f a dh lml l ���Ino�I  n > 8get name of folders of folder (homeordnerpfad as string)   o �pp p g e t   n a m e   o f   f o l d e r s   o f   f o l d e r   ( h o m e o r d n e r p f a d   a s   s t r i n g )m qrq l ��stus r  ��vwv n  ��xyx 1  ���H
�H 
pnamy n  ��z{z 2 ���G
�G 
file{ 4  ���F|
�F 
cfol| l ��}�E�D} c  ��~~ o  ���C�C 0 homeordnerpfad   m  ���B
�B 
TEXT�E  �D  w o      �A�A 
0 inhalt  t  without invisibles   u ��� $ w i t h o u t   i n v i s i b l e sr ��� l ���@���@  � # display dialog inhalt as text   � ��� : d i s p l a y   d i a l o g   i n h a l t   a s   t e x t� ��� l ���?���?  � 7 1repeat with i from 1 to number of items of inhalt   � ��� b r e p e a t   w i t h   i   f r o m   1   t o   n u m b e r   o f   i t e m s   o f   i n h a l t� ��>� X  ����=�� k  ���� ��� l ���<���<  � &  display dialog (dasFile) as text   � ��� @ d i s p l a y   d i a l o g   ( d a s F i l e )   a s   t e x t� ��;� Z  �����:�9� E  ����� l ����8�7� l ����6�5� o  ���4�4 0 dasfile dasFile�6  �5  �8  �7  � m  ���� ���  x c o d e p r o j� k  ���� ��� r  ����� b  ����� l ����3�2� c  ����� o  ���1�1 0 homeordnerpfad  � m  ���0
�0 
ctxt�3  �2  � l ����/�.� c  ����� o  ���-�- 0 dasfile dasFile� m  ���,
�, 
ctxt�/  �.  � o      �+�+ 0 filepfad  � ��� l ���*���*  � &  display dialog (dasFile) as text   � ��� @ d i s p l a y   d i a l o g   ( d a s F i l e )   a s   t e x t� ��)� I ���(��'
�( .aevtodocnull  �    alis� 4  ���&�
�& 
file� o  ���%�% 0 filepfad  �'  �)  �:  �9  �;  �= 0 dasfile dasFile� o  ���$�$ 
0 inhalt  �>   	 m     ���                                                                                  MACS  alis    r  Macintosh HD               �� �H+   �:
Finder.app                                                      Ƙh        ����  	                CoreServices    ǿ�      ƘK�     �:  ��  ��  3Macintosh HD:System:Library:CoreServices:Finder.app    
 F i n d e r . a p p    M a c i n t o s h   H D  &System/Library/CoreServices/Finder.app  / ��  ��  ��    ��#� l     �"�!� �"  �!  �   �#       ����  � �
� .aevtoappnull  �   � ****� �������
� .aevtoappnull  �   � ****� k    ���  ��  �  �  � ��� 0 i  � 0 dasfile dasFile� K�� ��������� B� I��
�	���������  ������������������������������������� �����������6��:����������������L[���������������
� .miscactvnull��� ��� obj � 0 filecontents fileContents
� 
alis
� 
rtyp
� 
ctxt
� .earsffdralis        afdr� 0 
homeordner  
� 
ctnr� 0 homeordnerpfad  
� 
TEXT� 0 filepfad  
� .sysodlogaskr        TEXT
�
 
file
�	 
perm
� .rdwropenshor       file� 0 refnum RefNum
� .rdwrread****        ****
� .misccurdldt    ��� null� 	0 heute  
� 
year� 0 jahrtext  
� 
mnth�  0 	monattext  
�� 
day ������ 0 tag  
�� 
jan 
�� 
feb 
�� 
mar 
�� 
apr 
�� 
may 
�� 
jun 
�� 
jul 
�� 
aug 
�� 
sep 
�� 
oct 
�� 
nov 
�� 
dec �� �� 0 monatsliste MonatsListe
�� 
cobj�� 	0 monat  
�� 
cha �� �� 0 jahr  �� 0 
neuesdatum 
neuesDatum
�� 
ret �� 0 	neuertext 	neuerText
�� 
set2
�� .rdwrseofnull���     ****
�� 
refn
�� .rdwrwritnull���     ****
�� .rdwrclosnull���     ****��  ��  �� 0 filetype  
�� 
cfol
�� 
pnam�� 
0 inhalt  
�� 
kocl
�� .corecnte****       ****
�� .aevtodocnull  �    alis����*j O�E�O*�)��l /E�O��,E�O��&�%E�O��%j O*j O*a �/a el E` O5_ j E�O*j E` O_ a ,E` O_ a ,E` Oa _ a ,%[�\[Za \Zi2E` Oa a  a !a "a #a $a %a &a 'a (a )a *a +vE` ,O :ka +kh  _ _ ,a -�/  a .�%[�\[Za \Zi2E` /OY h[OY��O_ �&[a 0\[Zm\Za 12E` 2O_ a 3%_ /%a 4%_ %E` 5Oa 6_ 5%_ 7%_ 7%_ 7%_ 7%_ 7%a 8%_ 7%�%_ 7%E` 9O_ a :jl ;O_ 9a <_ l =O_ j >W X ? @a Aj O_ j >Oa BkvE` CO*a D��&/a -a E,E` FO :_ F[a Ga -l Hkh �a I ��&��&%E�O*a �/j JY h[OY��Uascr  ��ޭ