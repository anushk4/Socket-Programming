all:
	$(MAKE) Q_1
	$(MAKE) Q_2
	
Q_1:
	cd Q1 && $(MAKE)

Q_2:
	cd Q2 && $(MAKE)

clean:
	@cd Q1 && $(MAKE) clean
	@cd Q2 && $(MAKE) clean
	@rm -f *~ *.o *.exe *perf*