# Makefile για τις βιβλιοθήκες k08.a και libbmp.a

# Βρίσκουμε ποιο λειτουργικό σύστημα χρησιμοποιείται
ifeq ($(OS),Windows_NT)
	SUFFIX = win
else ifeq ($(shell uname -s),Linux)
	SUFFIX = linux
else
	SUFFIX = macos
endif

all: k08.a libbmp.a

# "Παράγουμε" το k08.a απλά αντιγράφοντας το αρχείο που αντιστοιχεί στο λειτουργικό σύστημα που χρησιμοποιούμε
k08.a:
	cp k08_$(SUFFIX).a k08.a

# Το ίδιο για το libbmp.a
libbmp.a:
	cp libbmp_$(SUFFIX).a libbmp.a

clean:
	$(RM) k08.a libbmp.a