C
C     ****** SET CHARACTER ATTRIBUTE (HARDWARE) ******
C
      SUBROUTINE SETCHH(CHH,ANGL)
C
      CALL SETCHS(CHH,ANGL)
C      write(0,*) "GSAF/OpenGL Warning : SETCHH IS NOT SUPPORTED"
      CALL SETFNT(0)
C     CALL SETFNT(1)
      RETURN
      END
C
C     ****** GET CURSOR POSITION ******
C
      SUBROUTINE XYIN(X,Y)
      write(0,*) "GSAF/OpenGL Warning : XYIN IS NOT SUPPORTED"
      RETURN
      END
C
C     ****** CHANGE TO GRAPHIC SCREEN ******
C
      SUBROUTINE GRMODE
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
C
      IF(.NOT.LPAGE) RETURN
      WRITE(0,*) 'GSAF/OpenGL Warning : GRMODE IS NOT SUPPORTED'
C
C      CALL DVGRMD
      RETURN
      END
C
C     ******CHANGE TO CHARACTER SCREEN ******
C
      SUBROUTINE CHMODE
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
C
      IF(.NOT.LPAGE) RETURN
C
      WRITE(0,*) 'GSAF/OpenGL Warning : CHMODE IS NOT SUPPORTED'
C      CALL DVCHMD
      RETURN
      END
C
C     ****** START GROUP ******
C
      SUBROUTINE GUGRPS
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
C      COMMON /GSGRPL/ NGRPL,NGRPM
C
      IF(.NOT.LPAGE) RETURN
C      CALL DVGRPS
C      IF(LFIL) CALL BUFFST(8,0,0,0,0.0,0.0)
C      NGRPL=NGRPL+1
C      IF(NGRPL.GT.NGRPM) NGRPM=NGRPL
      RETURN
      END
C
C     ****** END GROUP ******
C
      SUBROUTINE GUGRPE
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
C      COMMON /GSGRPL/ NGRPL,NGRPM
C
      IF(.NOT.LPAGE) RETURN
C      CALL DVGRPE
C      IF(LFIL) CALL BUFFST(8,1,0,0,0.0,0.0)
C      NGRPL=NGRPL-1
      RETURN
      END
C
C     ****** INQUIRE GROUP LEVEL ******
C
      SUBROUTINE INQGRP(N1,N2)
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
C      COMMON /GSGRPL/ NGRPL,NGRPM
C
      IF(.NOT.LGSAF) RETURN
C
C      N1=NGRPL
C      N2=NGRPM
      N1 = 0
      N2 = 0
      RETURN
      END
C
C     ****** SET EVENT_MASK TYPE ******
C
      SUBROUTINE GF_SET_EVENT(ID)
C
C        ID = 0 : NO EVENT
C        ID = 1 : KEY PRESSED
C        ID = 2 : MOUSE PRESSED
C        ID = 4 : MOUSE RELEASED
C        ID = 8 : MOUSE MOTION
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
      COMMON /GSAFEV/ ID_EVENT
C
      IF(.NOT.LGSAF) RETURN
      IF(.NOT.LPAGE) RETURN
C
      WRITE(0,*) 'GSAF/OpenGL Warning : GF_SET_EVENT IS NOT SUPPORTED'
C      CALL DVSETV(ID)
      ID_EVENT=ID
      RETURN
      END
C
C     ****** INQUIRE EVENT_MASK TYPE ******
C
      SUBROUTINE GF_INQ_EVENT(ID)
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
      COMMON /GSAFEV/ ID_EVENT
C
      IF(.NOT.LGSAF) RETURN
      IF(.NOT.LPAGE) RETURN
C
      ID=ID_EVENT
C
      WRITE(0,*) 'GSAF/OpenGL Warning : GF_INQ_EVENT IS NOT SUPPORTED'
C
      RETURN
      END
C
C     ****** GET EVENT DATA ******
C
      SUBROUTINE GF_GET_EVENT(ID,X,Y,KID,KEY)
C
C        ID : 1 KeyPressed
C        ID : 2 MousePressed
C        ID : 4 MouseReleased
C        ID : 8 MouseMoution
C
C        X   : Mouse X Positon
C        Y   : Mouse Y Positon
C        KID : Key   : Character Code in ASCII
C              Mouse : Mouse Button Number
C        KEY : Key   : Key Number
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
C      COMMON /GSAFXY/ XDEL,YDEL,XORG,YORG
C
      IF(.NOT.LGSAF) RETURN
      IF(.NOT.LPAGE) RETURN
C
      WRITE(0,*) 'GSAF/OpenGL Warning : GF_GET_EVENT IS NOT SUPPORTED'
C      CALL gsglFlush
C      CALL DVGETV(ID,X,Y,KEY,KID)
C      X=(REAL(IX)+0.5-XORG)/XDEL
C      Y=(REAL(IY)+0.5-YORG)/YDEL
C
      RETURN
      END
C
C     ****** SET GC FUNCTION ******
C
      SUBROUTINE GF_SET_GCFUNC(ID)
C
C        ID =  0 : Clear             0
C        ID =  1 : And               S  &  D
C        ID =  2 : AndReverse        S  & !D
C        ID =  3 : Copy (Default)    S 
C        ID =  4 : AndInverted      !S  &  D
C        ID =  5 : Noop                    D
C        ID =  6 : Xor               S XOR D
C        ID =  7 : Or                S  |  D
C        ID =  8 : Nor              !S  & !D
C        ID =  9 : Equiv            !S XOR D
C        ID = 10 : Invert                 !D
C        ID = 11 : OrReverse         S  | !D
C        ID = 12 : CopyInverted     !S
C        ID = 13 : OrInverted       !S  |  D
C        ID = 14 : Nand             !S  | !D
C        ID = 15 : Set               1
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
      COMMON /GSAFGC/ ID_GCFUNC
C
      IF(.NOT.LGSAF) RETURN
      IF(.NOT.LPAGE) RETURN
C
      WRITE(0,*) 'GSAF/OpenGL Warning : GF_SET_GCFUNC IS NOT SUPPORTED'
C      CALL DVGCFUNC(ID)
C      IF(LFIL) CALL BUFFST(9,ID,0,0,0.0,0.0)
      ID_GCFUNC=ID
      RETURN
      END
C
C     ****** INQ GC FUNCTION ******
C
      SUBROUTINE GF_INQ_GCFUNC(ID)
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
      COMMON /GSAFGC/ ID_GCFUNC
C
      IF(.NOT.LGSAF) RETURN
      IF(.NOT.LPAGE) RETURN
C
      WRITE(0,*) 'GSAF/OpenGL Warning : GF_INQ_GCFUNC IS NOT SUPPORTED'
      ID=ID_GCFUNC
      RETURN
      END
C
C     ****** SYNC XWINDOW ******
C
      SUBROUTINE GF_SYNC
C
      IMPLICIT LOGICAL(L)
      COMMON /GSAFLG/ LGSAF,LPAGE,LKEEP,NPAGE,NHEAD
C
      IF(.NOT.LGSAF) RETURN
      IF(.NOT.LPAGE) RETURN
C
      WRITE(0,*) 'GSAF/OpenGL Warning : GF_SYNC IS NOT SUPPORTED'
C      CALL DVSYNC
      RETURN
      END
C     *************************************************
C     *         GSAF 3D ROUTINES : Color plot         *
C     *************************************************
C     OBSOLUTE : USE PERS3D1 INSTED.
      SUBROUTINE CPLOT3D1 (Z, NMX, NX, NY, IND, RGBFUNC)
C     colored plot of 3d graph
C     if IND=0, draw poly and lines
C     if IND=1, draw poly only
C     if IND=2, draw poly and x-lines only
C     if IND=3, draw poly and y-lines only
      DIMENSION Z(NMX,NY)
      EXTERNAL  RGBFUNC
      WRITE(0,*) 'GSAF/OpenGL Warning : CPLOT3D1 IS OBSOLUTE. PLEASE US'
     &'E PERS3D1 INSTEAD'
      Call Pers3d1 (Z, NMX, NX, NY, IND, RGBFUNC)
      return
      end
C
C
C
      SUBROUTINE PERSE1(Z,NXA,NXX,NYY,ZMIN,ZMAX,
     &                  IXY,IND,XL,YL,ZL,A,B,C,D,E)
      WRITE(0,*) 'GSAF/OpenGL Warnign : PERSE1 IS OBSOLUTE. PLEASE USE '
     &'PERS3D1 INSTEAD' 
      RETURN
      END
C
C
C
      SUBROUTINE GDATA3D1 ()
      RETURN
      END
C
C
C
      SUBROUTINE GDATA3D2 ()
      RETURN
      END
C
C
C
      SUBROUTINE GDATA3D3 ()
      RETURN
      END
C
C
C
      SUBROUTINE PERSE3D ()
      RETURN
      END
C
C     ****** SET OPTION ******
C
      SUBROUTINE GSOPTN(KOPTDL,IOPT)
      CHARACTER KOPTDL*4,KOPT*2
      RETURN
      END
C
C     ****** GET INPUT CHARACTERS ******
C
      SUBROUTINE CHIN(KTEXT,NCHAR1)
      CHARACTER KTEXT*256
      END
C
C
C
      SUBROUTINE GDRWBK(R,G,B)
      CALL GDRAWBACK3D(R,G,B)
      RETURN
      END
 
C     $Id: Aobsolute.f,v 1.13 2012/11/13 12:21:55 akutsu Exp $
C
C     ****** CONTOUR PLOT : XY, FIXED STEP, PATTERN ******
C
      SUBROUTINE CONTQ3D1(ZORG,ZSTEP,NSTEP,IPRD,IPAT,KA,RGBFUNC,IZ)
C
      EXTERNAL RGBFUNC
      EXTERNAL CONTS3D1
      DIMENSION KA(2,*)
      RETURN
      END
C
C     ****** CONTOUR PLOT : COMMON SUB (4 TRIANGLE) ******
C
      SUBROUTINE CONTQ3D0(X,Y,ZORG,ZSTEP,NSTEP,IPRD,IPAT,KA,SUB,RGBFUNC)
C
      IMPLICIT LOGICAL(L)
C
      EXTERNAL SUB
      EXTERNAL RGBFUNC
      DIMENSION X(*),Y(*),KA(2,*)
      DIMENSION RGB(3)
      RETURN
      END
C
C     ****** CONTOUR PLOT SLAVE ROUTINE : XY FIXED ******
C
      SUBROUTINE CONTS3D1(NAX,NAY,NBX,NBY,U0,UA,UB,X,Y,IPAT,IND)
C
      DIMENSION X(*),Y(*)
      RETURN
      END
