% 	SuperCollider real time audio synthesis system
%  Copyright (c) 2002 James McCartney. All rights reserved.
% 	http://www.audiosynth.com
%
%  This program is free software; you can redistribute it and/or modify
%  it under the terms of the GNU General Public License as published by
%  the Free Software Foundation; either version 2 of the License, or
%  (at your option) any later version.
%
%  This program is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU General Public License for more details.
%
%  You should have received a copy of the GNU General Public License
%  along with this program; if not, write to the Free Software
%  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA


% Brown/puckette constant Q transform, generating spectral kernels to a
% .wav file for loading into a Buffer for SC3 plug-in!

g_SR=48000; %can get from inTable somehow?
g_Nyquist=g_SR/2; %22050;
g_N= 4096; %4096;
g_Nover2=g_N/2; %2048;
%g_overlap=3072;
%g_overlapindex=1024;
%g_framespersec= g_overlap/g_SR;

g_log2N = log2(g_N);

%quartertones
g_fmin=174.6; %65.4064; %base C2, 2 octaves below middle C
scaleexp= 1.0293022366435;

g_qbands= floor(24*log2(g_Nyquist/g_fmin)) + 1;

'Qitch by Nick Collins from algorithms by Judith Brown/Miller Puckette \n'

g_qfreqs = zeros(g_qbands+1,1);

%1 more than size so have guard element at top
for ii=1:1:g_qbands+1
	g_qfreqs(ii)= (scaleexp^(ii-1))* g_fmin;
end

%g_qfreqs(g_qbands+1)=g_Nyquist;

temporalkernel= zeros(g_N,g_qbands);
spectralkernel= zeros(g_N,g_qbands);

%calculate kernels, needs g_qbands FFTs for efficiency


%Q is 1/((1.0293022366435)-1) Q=34.127
%minfreqsep= g_fmin*(1.0293022366435-1)= 5.0985
%used to scale window sizes
minfreqsep=g_fmin*(scaleexp-1);

alpha= 25/46; %for Hamming window

for ii=1:1:g_qbands

    ii

	%windowing size inverse to bandwidth, where full size for ii=0
	Nk= floor((g_N*minfreqsep)/((g_qfreqs(ii+1))- (g_qfreqs(ii))));

    Nk

    Nkmult= g_N/Nk

	minn= floor((g_N-Nk)*0.5);
	maxn= floor((g_N+Nk)*0.5);

	wkcq= 2*pi*(g_qfreqs(ii)/g_SR); %digital frequency

	%calculate signal to transform, see eqn (7)

    %temporal kernel i
	for jj = 0:1:(g_N-1)

		if((jj>=minn) && (jj<=maxn))

			window= alpha - ((1-alpha)*cos(2*pi*(jj-minn)/Nk));

			argument= wkcq*(jj-g_Nover2);

            temporalkernel(jj+1,ii)= Nkmult*real(window*exp(i*argument));

		 else  %zeroes
			temporalkernel(jj+1,ii)=0;
        end

    end


end

%normalised by window length?

% Carry out a Forward FFT transform
spectralkernel= (1/g_N)*(fft(temporalkernel, g_N));

%scale factor recip of g_N see (5)

%plot first 30 temporal kernels

% figure(2)
%
% hold on;
%
% for ii=1:1:30
%
%     plot(0.2*(temporalkernel(:,ii))+ii);
%
% end
%
% hold off;
%
% %plot first 10 spectral kernels
%
% figure(3)
%
% hold on;
%
% for ii=1:1:g_qbands
%
%     plot((abs(spectralkernel(:,ii)))+ii);
%
% end
%
% hold off;

cd /Volumes/data/moreapps/SuperCollider3/build/

%need format SR N numbands numvals then for each band: freq startfftind cumulind numvals vals
% only store real values
Y=[];

minval=0.15; %could make 0.1 for more accuracy %*(1/g_N);

cumul=0;

for ii=1:1:g_qbands

    ii
    target= real(spectralkernel(1:g_Nover2,ii)); %just real part of first half taken
    %find first value>0.15, run till less than 0.15
    temp= find((abs(target)>minval));

    temp= temp(find(temp<g_Nover2)); %no high bin numbers please

    freq=g_qfreqs(ii);
    startind= temp(1)-1; %indexing from 0 for C
    numvals=length(temp);
    vals= target(temp);

   % Y= [Y [freq startind cumul numvals vals']];

    Y= [Y [freq startind numvals vals']];

    %cumul=cumul+numvals;
end

%Y= [g_SR g_N g_qbands cumul Y];
Y= [g_SR g_N g_qbands Y];


%floating point file, will be loaded into buffer
wavwrite(Y,g_SR,32,['QspeckernN' int2str(g_N) 'SR' int2str(g_SR) '.wav'])
