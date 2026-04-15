                          port = 28000;   % must match your C++/CLI sender
u = udpport("datagram", "IPV4", "LocalPort", port);
disp("MATLAB listening for UDP telemetry...");

%% ============================================================
%   DATA BUFFERS
%% ============================================================
time = [];

accX = []; accY = []; accZ = [];
angleX = []; angleY = []; angleZ = [];
tempIn = []; tempOut = [];
altitude = [];
g_load = []; v_total = [];

%% ============================================================
%   FIGURE 1 — 6‑SUBPLOT TELEMETRY DASHBOARD
%% ============================================================

figure(1); clf;
set(gcf, 'Position', [20 50 650 700]);   % left window

% 1) Altitude vs Time
subplot(3,2,1);
hAlt = plot(nan, nan, 'LineWidth', 1.5);
title("Altitude vs Time");
xlabel("Time (s)"); ylabel("Altitude (m)");
grid on;

% 2) Angles vs Time
subplot(3,2,2);
hAng = plot(nan,nan, nan,nan, nan,nan, 'LineWidth', 1.2);
title("Angles vs Time");
xlabel("Time (s)"); ylabel("Angle (deg)");
legend("Angle X","Angle Y","Angle Z", 'Location','southoutside', 'Orientation','horizontal', ...
       'Box','off', 'NumColumns',3);
grid on;

% 3) Temperatures vs Time
subplot(3,2,3);
hTmp = plot(nan,nan, nan,nan, 'LineWidth', 1.2);
title("Temperatures vs Time");
xlabel("Time (s)"); ylabel("Temperature (°C)");
legend("Inside","Outside", 'Location','southoutside', 'Orientation','horizontal', ...
       'Box','off', 'NumColumns',3);
grid on;

% 4) Ground Speed vs Time
subplot(3,2,4);
hSpeed = plot(nan,nan, 'LineWidth', 1.5);
title("Ground Speed vs Time");
xlabel("Time (s)"); ylabel("Ground Speed (m/s)");
grid on;

% 5) Linear Accelerations vs Time
subplot(3,2,5);
hAcc = plot(nan,nan, nan,nan, nan,nan, 'LineWidth', 1.2);
title("Linear Accelerations (without g) vs Time");
xlabel("Time (s)"); ylabel("Acceleration (m/s^2)");
legend("Acc X","Acc Y","Acc Z", 'Location','southoutside', 'Orientation','horizontal', ...
       'Box','off', 'NumColumns',3);
grid on;

% 6) Total g-Load vs Time
subplot(3,2,6);
hLoad = plot(nan,nan, 'LineWidth', 1.5);
title("Total g-Load vs Time");
xlabel("Time (s)"); ylabel("g-Load (g)");
grid on;


%% ============================================================
%   FIGURE 2 — 3D CANSAT TUBE ORIENTATION (SOLID + AXES)
%% ============================================================

figure(2); clf;
xlim([-5 5]);
ylim([-5 5]);
zlim([-5 5]);
axis vis3d;   % prevents axis scaling during rotation
set(gcf, 'Position', [650 50 650 700]);   % right window

axis equal;
grid on;
xlabel('X'); ylabel('Y'); zlabel('Z');
view(3);
hold on;

% Tube parameters
radius = 1;
lengthTube = 4;

% Create cylinder aligned with X-axis
[YC, ZC, XC] = cylinder(radius, 40);
XC = XC * lengthTube - lengthTube/2;

% Tube vertices (Nx3)
tubeVertices = [XC(:), YC(:), ZC(:)];

% Draw tube surface
tubeSurf = surf(reshape(tubeVertices(:,1), size(XC)), ...
                reshape(tubeVertices(:,2), size(YC)), ...
                reshape(tubeVertices(:,3), size(ZC)));
shading interp;
colormap([0.6 0.6 1]);   % light blue

% --- Draw end caps ---
theta = linspace(0, 2*pi, 40);
circleY = radius * cos(theta);
circleZ = radius * sin(theta);

cap1 = fill3(-lengthTube/2 * ones(size(theta)), circleY, circleZ, [0.4 0.4 1]);
cap2 = fill3(+lengthTube/2 * ones(size(theta)), circleY, circleZ, [0.4 0.4 1]);

% --- Body axes (local coordinate frame) ---
axisLength = 3;
bodyAxes = [ 0 0 0;
             axisLength 0 0;
             0 axisLength 0;
             0 0 axisLength ];

axisX = plot3([0 axisLength], [0 0], [0 0], 'r', 'LineWidth', 2);
axisY = plot3([0 0], [0 axisLength], [0 0], 'g', 'LineWidth', 2);
axisZ = plot3([0 0], [0 0], [0 axisLength], 'b', 'LineWidth', 2);

% Rotation matrices
Rx = @(a)[1 0 0; 0 cosd(a) -sind(a); 0 sind(a) cosd(a)];
Ry = @(a)[cosd(a) 0 sind(a); 0 1 0; -sind(a) 0 cosd(a)];
Rz = @(a)[cosd(a) -sind(a) 0; sind(a) cosd(a) 0; 0 0 1];


%% ============================================================
%   MAIN LOOP — READ PACKETS + UPDATE BOTH FIGURES
%% ============================================================

tic;

try
    while true
        if u.NumDatagramsAvailable > 0
            d = read(u, 1, "double");   % read 1 datagram as doubles
            raw = d.Data;               % 9x1 double

            if numel(raw) ~= 11
                continue; % skip malformed packets
            end

            % Unpack
            accX(end+1) = raw(1);
            accY(end+1) = raw(2);
            accZ(end+1) = raw(3);

            angleX(end+1) = raw(4);
            angleY(end+1) = raw(5);
            angleZ(end+1) = raw(6);

            tempIn(end+1) = raw(7);
            tempOut(end+1) = raw(8);

            altitude(end+1) = raw(9);
            g_load(end + 1) = raw(10);
            v_total(end + 1) = raw(11);

            time(end+1) = toc;

            %% --- UPDATE FIGURE 1 ---
            set(hAlt, 'XData', time, 'YData', altitude);

            set(hAng(1), 'XData', time, 'YData', angleX);
            set(hAng(2), 'XData', time, 'YData', angleY);
            set(hAng(3), 'XData', time, 'YData', angleZ);

            set(hTmp(1), 'XData', time, 'YData', tempIn);
            set(hTmp(2), 'XData', time, 'YData', tempOut);

            set(hSpeed, 'XData', time, 'YData', v_total);

            set(hAcc(1), 'XData', time, 'YData', accX);
            set(hAcc(2), 'XData', time, 'YData', accY);
            set(hAcc(3), 'XData', time, 'YData', accZ);

            set(hLoad, 'XData', time, 'YData', g_load);

            %% --- UPDATE FIGURE 2 (3D CANSAT ORIENTATION) ---
            R = Rz(angleZ(end)) * Ry(angleY(end)) * Rx(angleX(end));

            % Rotate tube
            Vrot = (R * tubeVertices')';
            set(tubeSurf, ...
                'XData', reshape(Vrot(:,1), size(XC)), ...
                'YData', reshape(Vrot(:,2), size(YC)), ...
                'ZData', reshape(Vrot(:,3), size(ZC)));

            % Rotate end caps
            cap1Verts = (R * [-lengthTube/2 * ones(size(theta)); circleY; circleZ])';
            cap2Verts = (R * [+lengthTube/2 * ones(size(theta)); circleY; circleZ])';

            set(cap1, 'XData', cap1Verts(:,1), 'YData', cap1Verts(:,2), 'ZData', cap1Verts(:,3));
            set(cap2, 'XData', cap2Verts(:,1), 'YData', cap2Verts(:,2), 'ZData', cap2Verts(:,3));

            % Rotate axes
            axesRot = (R * bodyAxes')';

            set(axisX, 'XData', axesRot([1 2],1), 'YData', axesRot([1 2],2), 'ZData', axesRot([1 2],3));
            set(axisY, 'XData', axesRot([1 3],1), 'YData', axesRot([1 3],2), 'ZData', axesRot([1 3],3));
            set(axisZ, 'XData', axesRot([1 4],1), 'YData', axesRot([1 4],2), 'ZData', axesRot([1 4],3));

            drawnow limitrate;
        end
    end

catch ME
    disp("Telemetry loop stopped.");
    disp(ME.message);

finally
    disp("Closing TCP connection...");
    clear server;   % releases the port so you can rerun without restarting MATLAB
    disp("TCP connection closed.");
end