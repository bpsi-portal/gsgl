      program register_test
      implicit none
      real*4 data(10,3), size
      integer handler, i
C
      do i = 1, 10
         data(i,1) = i
         data(i,2) = i + 5
         data(i,3) = i * 10
      enddo
C
      call gsopen
      call pages
C
      call gdefin (0.0, 25.6, 0.0, 18.0, 5.0, 10.0, 15.0, 20.0)
      call register_data (handler, data(1,1), 1, 10,'xaxis','@data-1@')
      call register_data (handler, data(1,2), 1, 10, 'data','@data-2@')
      call register_data (handler, data(1,3), 1, 10, 'data','@data-3@')
      call plot_registered_data ()
C
      call pagee
      call gsclos
      return
      end
