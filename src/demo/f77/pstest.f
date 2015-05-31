C
      DIMENSION ND3(4)
      DATA ND3/32,33,36,40/
C
      CALL GSOPEN
      CALL PAGES
      DO N=0,12
         CALL MOVE(0.5,16.2-N*1.2)
         CALL SETFNT(0)
         CALL SETCHS(0.5,0.0)
         CALL NUMBI(N,'(I2)',2)
         CALL SETFNT(N)
         CALL SETCHS(0.8,0.0)
         CALL MOVE(2.0,16.2-N*1.2)
         CALL TEXT('ABCDEFGH',8)
         CALL MOVE(12.5,16.2-N*1.2)
         CALL TEXT('abcdefgh',8)
         CALL MOVE(21.0,16.2-N*1.2)
         CALL TEXT('1234',4)
      ENDDO
      CALL PAGEE
C
      CALL PAGES
      DO N=0,12
         CALL MOVE(0.5,16.2-N*1.2)
         CALL SETFNT(0)
         CALL SETCHS(0.5,0.0)
         CALL NUMBI(N+32,'(I2)',2)
         CALL SETFNT(N+32)
         CALL SETCHS(0.8,0.0)
         CALL MOVE(2.0,16.2-N*1.2)
         CALL TEXT('ABCDEFGH',8)
         CALL MOVE(12.5,16.2-N*1.2)
         CALL TEXT('abcdefgh',8)
         CALL MOVE(21.0,16.2-N*1.2)
         CALL TEXT('1234',4)
      ENDDO
      CALL PAGEE
C
      CALL PAGES
      Y=18.0
      DO N=1,4
         Y=Y-4.0
         CALL SETFNT(ND3(N))
         CALL SETCHS(0.3,0.0)
         CALL MOVE( 3.0,Y+2.4)
         CALL TEXT('NORMAL$$+SUPER$$=N.',19)
         CALL MOVE(13.0,Y+2.4)
         CALL TEXT('NORMAL$+SUPER$=N.',17)
         CALL MOVE( 3.0,Y+1.8)
         CALL TEXT('NORMAL$$-SUB$$=N.',17)
         CALL MOVE(13.0,Y+1.8)
         CALL TEXT('NORMAL$-SUB$=N.',15)
         CALL MOVE( 3.0,Y+1.2)
         CALL TEXT('NORMAL $$#abc$$# N.',19)
         CALL MOVE(13.0,Y+1.2)
         CALL TEXT('NORMAL $#abc$# N.',17)
         CALL MOVE( 3.0,Y+0.6)
         CALL TEXT('NORMAL $$0abc$$# N.',19)
         CALL MOVE(13.0,Y+0.6)
         CALL TEXT('NORMAL $0abc$# N.',17)
         CALL MOVE( 3.0,Y)
         CALL TEXT('NORMAL$$,A$$;B$$!C$$?DEF$$*BACK.',32)
         CALL MOVE(13.0,Y)
         CALL TEXT('NORMAL$,A$;B$!C$?DEF$*BACK.',27)
      ENDDO
      CALL MOVE( 3.0,1.0)
      CALL TEXT('$$# $$/264123123 $$#',20)
      CALL MOVE(13.0,1.0)
      CALL TEXT('$# $/264123123 $#',17)
      CALL PAGEE
C
      D=12345.0
      IFNT=32
      CALL PAGES
         CALL SETVEW(1.0,2.0,1.0,2.0,1.0,3.0,1.0,3.0)
         CALL SETFNT(IFNT)
         DO N=0,3
            CALL GNUMBR( 1.0,17.0-N,D,N,0)
            CALL GNUMBR(10.0,17.0-N,D,N,1)
            CALL GNUMBR(15.0,17.0-N,D,N,2)
            CALL GNUMBR(20.0,17.0-N,D,N,3)
         ENDDO
         DO N=0,3
            CALL GNUMBR( 1.0,12.0-N,D,-N,0)
            CALL GNUMBR(10.0,12.0-N,D,-N,1)
            CALL GNUMBR(15.0,12.0-N,D,-N,2)
            CALL GNUMBR(20.0,12.0-N,D,-N,3)
         ENDDO
         DO N=0,3
            CALL GNUMBR( 1.0, 7.0-N,D,-N-100,0)
            CALL GNUMBR(10.0, 7.0-N,D,-N-100,1)
            CALL GNUMBR(15.0, 7.0-N,D,-N-100,2)
            CALL GNUMBR(20.0, 7.0-N,D,-N-100,3)
         ENDDO
      CALL PAGEE
C
      IFNT=36
      CALL PAGES
C         CALL SETVEW(1.0,2.0,1.0,2.0,1.0,3.0,1.0,3.0)
         CALL SETFNT(IFNT)
         DO N=0,3
            CALL GNUMBR( 1.0,17.0-N,D,N,0)
            CALL GNUMBR(10.0,17.0-N,D,N,1)
            CALL GNUMBR(15.0,17.0-N,D,N,2)
            CALL GNUMBR(20.0,17.0-N,D,N,3)
         ENDDO
         DO N=0,3
            CALL GNUMBR( 1.0,12.0-N,D,-N,0)
            CALL GNUMBR(10.0,12.0-N,D,-N,1)
            CALL GNUMBR(15.0,12.0-N,D,-N,2)
            CALL GNUMBR(20.0,12.0-N,D,-N,3)
         ENDDO
         DO N=0,3
            CALL GNUMBR( 1.0, 7.0-N,D,-N-100,0)
            CALL GNUMBR(10.0, 7.0-N,D,-N-100,1)
            CALL GNUMBR(15.0, 7.0-N,D,-N-100,2)
            CALL GNUMBR(20.0, 7.0-N,D,-N-100,3)
         ENDDO
      CALL PAGEE
C
      CALL GSCLOS
      STOP
      END
