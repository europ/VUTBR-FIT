% ======================
% ISS
% 2016/2017
% Semestralny projekt
% xtotha01
% ======================


% ZAKOMENTOVANE RIADKY NIE SU SUCASTOU RIESENIA


% 1.)
[y, Fs]=audioread('xtotha01.wav');
% vzorkovacia frekvencia
Fs
% dlzka vo vzorkoch = pocet vzorkov
N=length(y);
N
% dlzka v sekundach
T=length(y)/Fs;
T


% 2.) IMAGE
% metoda1
	s=fft(y);
	subplot(321); plot(abs(s(1:Fs/2))); xlabel('f (Hz)'); ylabel('Magnitude');
% metoda2
	%G = 10 * log10(1/N * abs(fft(y)).^2);
	%f = (0:N/2-1)/N * Fs;
	%G = G(1:Fs/2);
	%subplot(321); plot(f,abs(G)); xlabel('f [Hz]'); ylabel('PSD [dB]');


% 3.) pozor na b-1
% metoda1
	[a,b]=max(abs(s(1:Fs/2)));
% metoda2
	%[a,b]=max(G);
max1_index=b-1;
max1_index


% 4.) IMAGE, stabilny
num=[0.2324 -0.4112 0.2324]; % b
den=[1 0.2289 0.4662]; % a
H=filt(num, den, 1/Fs);
subplot(322); pzmap(H);


% 5.) IMAGE, horna priepust
% metoda1
	% NEFUNGUJE subplot(323); freqz(num, den, Fs/2, Fs);
% metoda2
	f=(0:N/2-1)/N * Fs;
	G=freqz(num, den, Fs/2);
	subplot(323); plot(f,abs(G)); xlabel('f (Hz)'); ylabel('|H|');


% 6.) IMAGE
% metoda1
	x=filter(num, den, y);
	s2=fft(x);
	subplot(324); plot(abs(s2(1:Fs/2))); xlabel('f (Hz)'); ylabel('Magnitude');
% metoda2
	%x=filter(num, den, y);
	%G = 10 * log10(1/N * abs(fft(x)).^2);
	%f = (0:N/2-1)/N * Fs;
	%G = G(1:Fs/2);
	%subplot(324); plot(f,G); xlabel('f [Hz]'); ylabel ('PSD [dB]');



% 7.) pozor na d-1
% metoda1
	[c,d]=max(abs(s2(1:Fs/2)));
% metoda2
	%[c,d]=max(G);
max2_index=d-1;
max2_index


% 8.)
% vo for loop vytvorime vektor
for i = 1:80
	if (mod(i,2)==0)
		v(4*i-3) = [-1];
		v(4*i-2) = [-1];
		v(4*i-1) = [-1];
		v(4*i-0) = [-1];
	else
		v(4*i-3) = [+1];
		v(4*i-2) = [+1];
		v(4*i-1) = [+1];
		v(4*i-0) = [+1];
	end
end
% cez xcorr vyhladame v xtotha01.wav vektor
[acor,lag] = xcorr(y,v);
[~,I] = max(abs(acor));
lagDiff = lag(I);
timeDiff = lagDiff/Fs;
N_corel=lagDiff
T_corel=timeDiff


% 9.) IMAGE
Rv=xcorr(y, 'biased');
subplot(325); plot(-50:50, Rv(15950:16050)); xlabel('k'); ylabel('R[k]');



% 10.)
R_10=Rv(16010);
R_10


% 11.) 3D IMAGE
	%vector_null = [0;0;0;0;0;0;0;0;0;0];
	%vector_1 = [vector_null;y];
	%vector_2 = [y;vector_null];
	
	%vector_size = 100;
	%interval_start = -1;
	%interval_end = +1;
	%interval_length = 2/vector_size;
	
	%vector_3(1) = -1;
	%for j = 2:vector_size
	%	vector_3(j) = vector_3(j-1) + interval_length;
	%end
	
	%[h,p,r] = hist2opt(vector_1,vector_2,vector_3);
	
	%subplot(326); imagesc(h,p,r); axis xy; colorbar;

% 12.)
%TODO

% 13.)
%TODO

