EXE = eftpx
IMG = img
DAT = dat

$(EXE): $(IMG)
	cd src && $(MAKE) EXE=../$(EXE)

$(IMG): $(DAT)
	./saver.py

$(DAT):
	./scraper.py

purge: clean
	-@rm -r dat 2>/dev/null || true
	-@rm -r img 2>/dev/null || true
	-@rm -rf __pycache__ 2>/dev/null || true

clean:
	-@rm $(EXE) 2>/dev/null && cd src && $(MAKE) clean || true
