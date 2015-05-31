c
c     sample program
c
      program freetype
c      
      CALL gsopen
c
      CALL pages
      chh = 0.55
      CALL setchs (chh, 0.0)
c
      DO N = 1, 4
         IFNT = 501 + N
         CALL SETFNT(IFNT)
         IF (IFNT.EQ.502) THEN
            CALL MOVE (0.5, 20.2-3.7*1)
            CALL TEXTX ('@Times Roman (502)@')
         ELSE IF (IFNT.EQ.503) THEN
            CALL MOVE (0.5, 20.2-3.7*2)
            CALL TEXTX ('@Times Italic (503)@')
         ELSE IF (IFNT.EQ.504) THEN
            CALL MOVE (0.5, 20.2-3.7*3)
            CALL TEXTX ('@Times Bold (504)@')
         ELSE IF (IFNT.EQ.505) THEN
            CALL MOVE (0.5, 20.2-3.7*4)
            CALL TEXTX ('@Times Italic-Bold (505)@')
         ENDIF
         DO I=32,63
            CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
      ENDDO
C
      CALL pagee
      CALL pages
      chh = 0.55
      CALL setchs (chh, 0.0)
c
      DO N = 1, 4
         IFNT = 505 + N
         CALL SETFNT(IFNT)
         IF (IFNT.EQ.506) THEN
            CALL MOVE (0.5, 20.2-3.7*1)
            CALL TEXTX ('@Helvetica (506)@')
         ELSE IF (IFNT.EQ.507) THEN
            CALL MOVE (0.5, 20.2-3.7*2)
            CALL TEXTX ('@Helvetica Oblique (507)@')
         ELSE IF (IFNT.EQ.508) THEN
            CALL MOVE (0.5, 20.2-3.7*3)
            CALL TEXTX ('@Helvetica Bold (508)@')
         ELSE IF (IFNT.EQ.509) THEN
            CALL MOVE (0.5, 20.2-3.7*4)
            CALL TEXTX ('@Helvetica BoldOblique (509)@')
         ENDIF
         DO I=32,63
            CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
      ENDDO
C
      CALL pagee
      CALL pages
      chh = 0.55
      CALL setchs (chh, 0.0)
c
      DO N = 1, 4
         IFNT = 509 + N
         CALL SETFNT(IFNT)
         IF (IFNT.EQ.510) THEN
            CALL MOVE (0.5, 20.2-3.7*1)
            CALL TEXTX ('@Courier (510)@')
         ELSE IF (IFNT.EQ.511) THEN
            CALL MOVE (0.5, 20.2-3.7*2)
            CALL TEXTX ('@Courier Oblique (511)@')
         ELSE IF (IFNT.EQ.512) THEN
            CALL MOVE (0.5, 20.2-3.7*3)
            CALL TEXTX ('@Courier Bold (512)@')
         ELSE IF (IFNT.EQ.513) THEN
            CALL MOVE (0.5, 20.2-3.7*4)
            CALL TEXTX ('@Courier BoldOblique (513)@')
         ENDIF
         DO I=32,63
            CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
      ENDDO
C
      CALL pagee
      CALL pages
      chh = 0.55
      CALL setchs (chh, 0.0)
c
      DO N = 1, 1
         IFNT = 513 + N
         CALL SETFNT(IFNT)
         IF (IFNT.EQ.514) THEN
            CALL MOVE (0.5, 20.2-3.7*1)
            CALL TEXTX ('@Symbol (514)@')
         ENDIF
         DO I=32,63
            CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
C         DO I=160,191
C            CALL MOVE(1.0+chh*1.05*(I-160),16.8-N*3.7)
C            CALL TEXT(CHAR(I),1)
C         ENDDO
C         DO I=192,223
C            CALL MOVE(1.0+chh*1.05*(I-192),15.8-N*3.7)
C            CALL TEXT(CHAR(I),1)
C         ENDDO
C         DO I=240,271
C            CALL MOVE(1.0+chh*1.05*(I-240),14.8-N*3.7)
C            CALL TEXT(CHAR(I),1)
C         ENDDO
C         DO I=272,303
C            CALL MOVE(1.0+chh*1.05*(I-272),13.8-N*3.7)
C            CALL TEXT(CHAR(I),1)
C         ENDDO
C         DO I=304,335
C            CALL MOVE(1.0+chh*1.05*(I-304),12.8-N*3.7)
C            CALL TEXT(CHAR(I),1)
C         ENDDO
      ENDDO
C
      CALL pagee
      CALL pages
      chh = 0.55
      CALL setchs (chh, 0.0)
c
      DO N = 1, 4
         IFNT = 1001 + N
         CALL SETFNT(IFNT)
         IF (IFNT.EQ.1002) THEN
            CALL MOVE (0.5, 20.2-3.7*1)
            CALL TEXTX ('@Times Roman (1002)@')
         ELSE IF (IFNT.EQ.1003) THEN
            CALL MOVE (0.5, 20.2-3.7*2)
            CALL TEXTX ('@Times Italic (1003)@')
         ELSE IF (IFNT.EQ.1004) THEN
            CALL MOVE (0.5, 20.2-3.7*3)
            CALL TEXTX ('@Times Bold (1004)@')
         ELSE IF (IFNT.EQ.1005) THEN
            CALL MOVE (0.5, 20.2-3.7*4)
            CALL TEXTX ('@Times Italic-Bold (1005)@')
         ENDIF
         DO I=32,63
            CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
      ENDDO
C
      CALL pagee
      CALL pages
c     
      chh = 0.55
      CALL setchs (chh, 0.0)
C      CALL gsglVectorText ()
c
      DO N = 1, 4
         IFNT = 1005 + N
         CALL SETFNT(IFNT)
         IF (IFNT.EQ.1006) THEN
            CALL MOVE (0.5, 20.2-3.7*1)
            CALL TEXTX ('@Arial Roman (1006)@')
         ELSE IF (IFNT.EQ.1007) THEN
            CALL MOVE (0.5, 20.2-3.7*2)
            CALL TEXTX ('@Arial Italic (1007)@')
         ELSE IF (IFNT.EQ.1008) THEN
            CALL MOVE (0.5, 20.2-3.7*3)
            CALL TEXTX ('@Arial Bold (1008)@')
         ELSE IF (IFNT.EQ.1009) THEN
            CALL MOVE (0.5, 20.2-3.7*4)
            CALL TEXTX ('@Arial Italic-Bold (1009)@')
         ENDIF
         DO I=32,63
            CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
      ENDDO
C
      CALL pagee
      CALL pages
c     
      chh = 0.55
      CALL setchs (chh, 0.0)
c      CALL gsglVectorText ()
c
      DO N = 1, 4
         IFNT = 1009 + N
         CALL SETFNT(IFNT)
         IF (IFNT.EQ.1010) THEN
            CALL MOVE (0.5, 20.2-3.7*1)
            CALL TEXTX ('@Courier Roman (1010)@')
         ELSE IF (IFNT.EQ.1011) THEN
            CALL MOVE (0.5, 20.2-3.7*2)
            CALL TEXTX ('@Courier Italic (1011)@')
         ELSE IF (IFNT.EQ.1012) THEN
            CALL MOVE (0.5, 20.2-3.7*3)
            CALL TEXTX ('@Courier Bold (1012)@')
         ELSE IF (IFNT.EQ.1013) THEN
            CALL MOVE (0.5, 20.2-3.7*4)
            CALL TEXTX ('@Courier Italic-Bold (1013)@')
         ENDIF
         DO I=32,63
            CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
      ENDDO
C
      CALL pagee
      CALL pages
c     
      chh = 0.55
      CALL setchs (chh, 0.0)
C      CALL gsglVectorText ()
c
      DO N = 1, 4
         IFNT = 1014 + N
         CALL SETFNT(IFNT)
         IF (IFNT.EQ.1015) THEN
            CALL MOVE (0.5, 20.2-3.7*1)
            CALL TEXTX ('@Greek Roman (1015)@')
         ELSE IF (IFNT.EQ.1016) THEN
            CALL MOVE (0.5, 20.2-3.7*2)
            CALL TEXTX ('@Greek Italic (1016)@')
         ELSE IF (IFNT.EQ.1017) THEN
            CALL MOVE (0.5, 20.2-3.7*3)
            CALL TEXTX ('@Greek Bold (1017)@')
         ELSE IF (IFNT.EQ.1018) THEN
            CALL MOVE (0.5, 20.2-3.7*4)
            CALL TEXTX ('@Greek Italic-Bold (1018)@')
         ENDIF
         DO I=32,63
            CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
      ENDDO
C
      CALL pagee
      CALL pages
c     
      chh = 0.55
      CALL setchs (chh, 0.0)
C      CALL gsglVectorText ()
c
      CALL SETFNT (1020)
      CALL MOVE (0.5, 20.2-3.7*1)
      CALL TEXT ('日本語明朝体 (1020)', 19)
      CALL MOVE (1.0, 19.2-3.7*1)
      CALL TEXT (
     &     'あいうえおかきくけこさしすせそたちつてとなにぬねの', 50)
      CALL MOVE (1.0, 18.2-3.7*1)
      CALL TEXT (
     &     'はひふへほまみむめもやいゆえよらりるれろわゐゑをん', 50)
      CALL MOVE (1.0, 17.2-3.7*1)
      CALL TEXT (
     &     'アイウエオカキクケコサシスセソタチツテトナニヌネノ', 50)
      CALL MOVE (1.0, 16.2-3.7*1)
      CALL TEXT (
     &     'ハヒフヘホマミムメモヤイユエヨラリルレロワヰヱヲン', 50)
      CALL MOVE (1.0, 15.2-3.7*1)
      CALL TEXT (
     &     '微分積分線形代数古典力学電磁気解析力学確率統計量子', 50)
      CALL MOVE (1.0, 14.2-3.7*1)
      CALL TEXT (
     &     '日本語の文字数は１文字２バイトであることを考慮して', 50)

      CALL SETFNT (1021)
      CALL MOVE (0.5, 20.2-3.7*3)
      CALL TEXT ('日本語ゴシック体 (1021)', 23)
      CALL MOVE (1.0, 19.2-3.7*3)
      CALL TEXT (
     &     'あいうえおかきくけこさしすせそたちつてとなにぬねの', 50)
      CALL MOVE (1.0, 18.2-3.7*3)
      CALL TEXT (
     &     'はひふへほまみむめもやいゆえよらりるれろわゐゑをん', 50)
      CALL MOVE (1.0, 17.2-3.7*3)
      CALL TEXT (
     &     'アイウエオカキクケコサシスセソタチツテトナニヌネノ', 50)
      CALL MOVE (1.0, 16.2-3.7*3)
      CALL TEXT (
     &     'ハヒフヘホマミムメモヤイユエヨラリルレロワヰヱヲン', 50)
      CALL MOVE (1.0, 15.2-3.7*3)
      CALL TEXT (
     &     '微分積分線形代数古典力学電磁気解析力学確率統計量子', 50)
      CALL MOVE (1.0, 14.2-3.7*3)
      CALL TEXT (
     &     '日本語の文字数は１文字２バイトであることを考慮して', 50)
      CALL pagee
      CALL pages
c     
      chh = 0.55
      CALL setchs (chh, 0.0)
C      CALL gsglVectorText ()
c
      DO N = 1, 4
         IFNT = 1021 + N
         CALL SETFNT(IFNT)
         IF (IFNT.EQ.1022) THEN
            CALL MOVE (0.5, 20.2-3.7*1)
            CALL TEXTX ('@Computer Modern Roman (1022)@')
         ELSE IF (IFNT.EQ.1023) THEN
            CALL MOVE (0.5, 20.2-3.7*2)
            CALL TEXTX ('@Computer Modern Text Italic (1023)@')
         ELSE IF (IFNT.EQ.1024) THEN
            CALL MOVE (0.5, 20.2-3.7*3)
            CALL TEXTX ('@Computer Modern Typewriter Type (1024)@')
         ELSE IF (IFNT.EQ.1025) THEN
            CALL MOVE (0.5, 20.2-3.7*4)
            CALL TEXTX ('@Computer Modern Math Italic (1025)@')
         ENDIF
         DO I=32,63
            CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=64,95
            CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
         DO I=96,127
            CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*3.7)
            CALL TEXT(CHAR(I),1)
         ENDDO
      ENDDO
C
      CALL pagee
      CALL pages
c     
      chh = 0.55
      CALL setchs (chh, 0.0)
C      CALL gsglVectorText ()
c
      N = 1
      IFNT = 1025 + N
      CALL SETFNT(IFNT)
      CALL MOVE (0.5, 20.2-4.0*1)
      CALL TEXTX ('@$0Computer Modern Math Symbols (1026)@')
      DO I=32,63
         CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*4.0)
         CALL TEXT(CHAR(I),1)
      ENDDO
      DO I=64,95
         CALL MOVE(1.0+chh*1.05*(I-64),18.5-N*4.0)
         CALL TEXT(CHAR(I),1)
      ENDDO
      DO I=96,127
         CALL MOVE(1.0+chh*1.05*(I-96),17.8-N*4.0)
         CALL TEXT(CHAR(I),1)
      ENDDO
      N = 2
      IFNT = 1025 + N
      CALL SETFNT(IFNT)
      CALL MOVE (0.5, 20.2-4.0*2)
      CALL TEXTX ('@$0Computer Modern Math Extension (1027)@')
      DO I=32,63
         CALL MOVE(1.0+chh*1.05*(I-32),19.2-N*4.0)
         CALL TEXT(CHAR(I),1)
      ENDDO
      DO I=64,95
         CALL MOVE(1.0+chh*1.05*(I-64),16.5-N*4.0)
         CALL TEXT(CHAR(I),1)
      ENDDO
      DO I=96,127
         CALL MOVE(1.0+chh*1.05*(I-96),13.8-N*4.0)
         CALL TEXT(CHAR(I),1)
      ENDDO
C
      CALL pagee
      CALL pages
c     
      chh = 0.55
      CALL setchs (chh, 0.0)
c
      CALL SETFNT (1000)
      CALL MOVE (1.0, 19.2-2.7*1)
      CALL TEXT ('$$sqrt{ABC}=$sqrt{ABC}', 22)
      CALL MOVE (1.0, 18.2-2.7*1)
      CALL TEXT ('$$frac{A}{B}=$frac{A}{B}', 24)
      CALL MOVE (1.0, 17.2-2.7*1)
      CALL SETCHR (chh, chh, chh, 0.0, -20.0)
      CALL TEXT ('TILT=-20.0 ABCDEFG', 18)
      CALL MOVE (1.0, 16.2-2.7*1)
      CALL SETCHR (chh, chh, chh, -45.0, 0.0)
      CALL TEXT ('ANGL=-45.0 ABCDEFG', 18)
      CALL MOVE (1.0, 15.2-2.7*1)
      CALL SETCHR (chh, chh, chh, 90.0, 0.0)
      CALL MOVE (4.0, 4.0)
      CALL TEXT (
     &'y(x) = $frac{$del}{$del x}$left($frac{$del E}{$del x}$right)',60)
      CALL pagee
      CALL pages
c     
      chh = 0.55
      CALL setchs (chh, 0.0)
c
      CALL SETFNT (1020)
      CALL MOVE (1.0, 19.2-3.7*1)
      CALL TEXT (
     &'日本語用やギリシャ語用のフォントが設定されている状態で', 54)
      CALL MOVE (1.0, 18.2-3.7*1)
      CALL TEXT (
     &'英数字が使われた場合，1000番のフォントが使われる．ま', 54)
      CALL MOVE (1.0, 17.2-3.7*1)
      CALL TEXT (
     &'た，英語やギリシャ語用のフォントが設定されていて日本語が', 54)
      CALL MOVE (1.0, 16.2-3.7*1)
      CALL TEXT (
     &'使われた場合1019番のフォントが使われる．ギリシャ文字', 54)
      CALL MOVE (1.0, 15.2-3.7*1)
      CALL TEXT (
     &'を表示するには$$#で囲えばよい．例えば$$#abc$$#と入力す', 54)
      CALL MOVE (1.0, 14.2-3.7*1)
      CALL TEXT (
     &'ると$#abc$#と表示される．$${14}abcと入力しても同じ結果が', 56)
      CALL MOVE (1.0, 13.2-3.7*1)
      CALL TEXT (
     &'得られる．添え字を表示するには$$+または$$-で始め，$$=で',58)
      CALL MOVE (1.0, 12.2-3.7*1)
      CALL TEXT (
     &'終わる．例えばx$$+2$$=と入力すればx$+2$=となり，x$$-1$$=と',60)
      CALL MOVE (1.0, 11.2-3.7*1)
      CALL TEXT (
     &'入力するとx$-1$=となる．x$$+2$$=$$-1$$=と入力すればx$+2$=$-1$= と
     &なる．', 77)
      CALL MOVE (1.0, 10.2-3.7*1)
      CALL TEXT (
     &'添え字は何重にも表示できて，x$-a$-b$-c$=$=$= ということもできる．
     &', 65)
      CALL pagee
c
      CALL gsclos
c      
      stop
      end
c
c
c
      subroutine draw_box (xmin, xmax, ymin, ymax)
c
      CALL move (xmin, ymin)
      CALL draw (xmin, ymax)
      CALL draw (xmax, ymax)
      CALL draw (xmax, ymin)
      CALL draw (xmin, ymin)
c     
      return
      end
      
