all:
	cd 'Concurrent TCP' && $(MAKE)
	cd 'Select TCP' && $(MAKE)
	cd 'Single Threaded TCP' && $(MAKE)

clean:
	@cd 'Concurrent TCP' && $(MAKE) clean
	@cd 'Select TCP' && $(MAKE) clean
	@cd 'Single Threaded TCP' && $(MAKE) clean
	@rm -f *~ *.o *.exe *perf*