import { useEffect, useState } from 'react';
import { 
  Box, 
  Grid, 
  Paper, 
  Typography, 
  Stack,
  Card,
  CardContent,
  CardHeader,
  CircularProgress,
  Divider,
  useTheme
} from '@mui/material';
import { 
  DevicesOther as DevicesIcon,
  Cloud as CloudIcon,
  PowerOff as PowerOffIcon,
  SignalWifi4Bar as WifiIcon,
  ToggleOn as ToggleIcon
} from '@mui/icons-material';
import { 
  LineChart, 
  Line, 
  BarChart,
  Bar,
  XAxis, 
  YAxis, 
  CartesianGrid, 
  Tooltip, 
  Legend, 
  ResponsiveContainer,
  PieChart,
  Pie,
  Cell
} from 'recharts';

import useDeviceStore from '../store/deviceStore';
import { ControlType } from '../types/heep';

// Example temperature data
const tempData = [
  { time: '00:00', temp: 22.1 },
  { time: '01:00', temp: 21.8 },
  { time: '02:00', temp: 21.5 },
  { time: '03:00', temp: 21.3 },
  { time: '04:00', temp: 21.0 },
  { time: '05:00', temp: 20.8 },
  { time: '06:00', temp: 20.9 },
  { time: '07:00', temp: 21.2 },
  { time: '08:00', temp: 21.7 },
  { time: '09:00', temp: 22.3 },
  { time: '10:00', temp: 22.8 },
  { time: '11:00', temp: 23.2 },
  { time: '12:00', temp: 23.5 },
];

// Example humidity data
const humidityData = [
  { time: '00:00', humidity: 45 },
  { time: '02:00', humidity: 46 },
  { time: '04:00', humidity: 47 },
  { time: '06:00', humidity: 48 },
  { time: '08:00', humidity: 47 },
  { time: '10:00', humidity: 46 },
  { time: '12:00', humidity: 45 },
];

const Dashboard = () => {
  const theme = useTheme();
  const { loadDevices, getAllDevices, getOnlineDevices, getOfflineDevices, isLoading } = useDeviceStore();
  const [controlTypeCounts, setControlTypeCounts] = useState<Record<string, number>>({});
  
  useEffect(() => {
    loadDevices();
  }, [loadDevices]);
  
  useEffect(() => {
    const devices = getAllDevices();
    
    // Count the different types of controls
    const counts: Record<string, number> = {};
    
    devices.forEach(device => {
      device.controls.forEach(control => {
        counts[control.type] = (counts[control.type] || 0) + 1;
      });
    });
    
    setControlTypeCounts(counts);
  }, [getAllDevices]);
  
  // Prepare data for the devices pie chart
  const deviceStatusData = [
    { name: 'Online', value: getOnlineDevices().length },
    { name: 'Offline', value: getOfflineDevices().length }
  ];
  
  // Prepare data for the control types bar chart
  const controlTypeData = Object.entries(controlTypeCounts).map(([type, count]) => ({
    type,
    count
  }));
  
  // Colors for the pie charts
  const COLORS = [theme.palette.primary.main, theme.palette.error.main];
  
  if (isLoading) {
    return (
      <Box sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '100%' }}>
        <CircularProgress />
      </Box>
    );
  }
  
  return (
    <Box sx={{ flexGrow: 1, p: 2 }}>
      <Typography variant="h4" gutterBottom>
        Dashboard
      </Typography>
      
      {/* Summary Cards */}
      <Grid container spacing={3} sx={{ mb: 4 }}>
        <Grid item xs={12} sm={6} md={3}>
          <Paper elevation={3} sx={{ p: 2, display: 'flex', alignItems: 'center' }}>
            <DevicesIcon sx={{ fontSize: 40, mr: 2, color: theme.palette.primary.main }} />
            <Stack>
              <Typography variant="body2" color="text.secondary">
                Total Devices
              </Typography>
              <Typography variant="h4">
                {getAllDevices().length}
              </Typography>
            </Stack>
          </Paper>
        </Grid>
        
        <Grid item xs={12} sm={6} md={3}>
          <Paper elevation={3} sx={{ p: 2, display: 'flex', alignItems: 'center' }}>
            <WifiIcon sx={{ fontSize: 40, mr: 2, color: theme.palette.success.main }} />
            <Stack>
              <Typography variant="body2" color="text.secondary">
                Online Devices
              </Typography>
              <Typography variant="h4">
                {getOnlineDevices().length}
              </Typography>
            </Stack>
          </Paper>
        </Grid>
        
        <Grid item xs={12} sm={6} md={3}>
          <Paper elevation={3} sx={{ p: 2, display: 'flex', alignItems: 'center' }}>
            <PowerOffIcon sx={{ fontSize: 40, mr: 2, color: theme.palette.error.main }} />
            <Stack>
              <Typography variant="body2" color="text.secondary">
                Offline Devices
              </Typography>
              <Typography variant="h4">
                {getOfflineDevices().length}
              </Typography>
            </Stack>
          </Paper>
        </Grid>
        
        <Grid item xs={12} sm={6} md={3}>
          <Paper elevation={3} sx={{ p: 2, display: 'flex', alignItems: 'center' }}>
            <ToggleIcon sx={{ fontSize: 40, mr: 2, color: theme.palette.warning.main }} />
            <Stack>
              <Typography variant="body2" color="text.secondary">
                Controls
              </Typography>
              <Typography variant="h4">
                {Object.values(controlTypeCounts).reduce((a, b) => a + b, 0)}
              </Typography>
            </Stack>
          </Paper>
        </Grid>
      </Grid>
      
      {/* Charts */}
      <Grid container spacing={3}>
        {/* Device Status Chart */}
        <Grid item xs={12} md={6}>
          <Card>
            <CardHeader title="Device Status" />
            <Divider />
            <CardContent>
              <ResponsiveContainer width="100%" height={300}>
                <PieChart>
                  <Pie
                    data={deviceStatusData}
                    cx="50%"
                    cy="50%"
                    labelLine={false}
                    outerRadius={100}
                    fill="#8884d8"
                    dataKey="value"
                    label={({ name, percent }) => `${name}: ${(percent * 100).toFixed(0)}%`}
                  >
                    {deviceStatusData.map((entry, index) => (
                      <Cell key={`cell-${index}`} fill={COLORS[index % COLORS.length]} />
                    ))}
                  </Pie>
                  <Tooltip formatter={(value) => [`${value} devices`, 'Count']} />
                  <Legend />
                </PieChart>
              </ResponsiveContainer>
            </CardContent>
          </Card>
        </Grid>
        
        {/* Control Types Chart */}
        <Grid item xs={12} md={6}>
          <Card>
            <CardHeader title="Control Types" />
            <Divider />
            <CardContent>
              <ResponsiveContainer width="100%" height={300}>
                <BarChart data={controlTypeData}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="type" />
                  <YAxis />
                  <Tooltip />
                  <Legend />
                  <Bar dataKey="count" fill={theme.palette.primary.main} />
                </BarChart>
              </ResponsiveContainer>
            </CardContent>
          </Card>
        </Grid>
        
        {/* Temperature Chart */}
        <Grid item xs={12} md={6}>
          <Card>
            <CardHeader title="Temperature (Last 12 Hours)" />
            <Divider />
            <CardContent>
              <ResponsiveContainer width="100%" height={300}>
                <LineChart data={tempData}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="time" />
                  <YAxis unit="°C" domain={[20, 24]} />
                  <Tooltip formatter={(value) => [`${value}°C`, 'Temperature']} />
                  <Legend />
                  <Line 
                    type="monotone" 
                    dataKey="temp" 
                    stroke={theme.palette.secondary.main} 
                    activeDot={{ r: 8 }} 
                  />
                </LineChart>
              </ResponsiveContainer>
            </CardContent>
          </Card>
        </Grid>
        
        {/* Humidity Chart */}
        <Grid item xs={12} md={6}>
          <Card>
            <CardHeader title="Humidity (Last 12 Hours)" />
            <Divider />
            <CardContent>
              <ResponsiveContainer width="100%" height={300}>
                <LineChart data={humidityData}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="time" />
                  <YAxis unit="%" domain={[40, 50]} />
                  <Tooltip formatter={(value) => [`${value}%`, 'Humidity']} />
                  <Legend />
                  <Line 
                    type="monotone" 
                    dataKey="humidity" 
                    stroke={theme.palette.info.main} 
                    activeDot={{ r: 8 }} 
                  />
                </LineChart>
              </ResponsiveContainer>
            </CardContent>
          </Card>
        </Grid>
      </Grid>
    </Box>
  );
};

export default Dashboard;
