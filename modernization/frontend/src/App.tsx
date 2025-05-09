import { useState, useEffect } from 'react';
import { 
  ThemeProvider, 
  createTheme, 
  CssBaseline, 
  Box 
} from '@mui/material';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import { QueryClient, QueryClientProvider } from '@tanstack/react-query';

// Import custom components
import NavBar from './components/NavBar';
import SideDrawer from './components/SideDrawer';
import Dashboard from './pages/Dashboard';
import DeviceList from './pages/DeviceList';
import DeviceDetails from './pages/DeviceDetails';
import Settings from './pages/Settings';
import NotFound from './pages/NotFound';

// Import theme and styles
import { lightTheme, darkTheme } from './theme';

// Create a client for React Query
const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      refetchOnWindowFocus: false,
      retry: 1,
      staleTime: 10 * 1000,
    },
  },
});

function App() {
  const [darkMode, setDarkMode] = useState(
    localStorage.getItem('theme') === 'dark'
  );
  const [drawerOpen, setDrawerOpen] = useState(false);
  
  const toggleDrawer = () => {
    setDrawerOpen(!drawerOpen);
  };
  
  const toggleTheme = () => {
    const newMode = !darkMode;
    setDarkMode(newMode);
    localStorage.setItem('theme', newMode ? 'dark' : 'light');
  };
  
  // Selected theme based on dark mode state
  const theme = createTheme(darkMode ? darkTheme : lightTheme);
  
  return (
    <QueryClientProvider client={queryClient}>
      <ThemeProvider theme={theme}>
        <CssBaseline />
        <Router>
          <Box sx={{ display: 'flex' }}>
            <NavBar 
              toggleDrawer={toggleDrawer}
              darkMode={darkMode}
              toggleTheme={toggleTheme}
            />
            <SideDrawer 
              open={drawerOpen} 
              toggleDrawer={toggleDrawer} 
            />
            <Box 
              component="main" 
              sx={{ 
                flexGrow: 1, 
                p: 3, 
                mt: 8,
                overflow: 'hidden',
              }}
            >
              <Routes>
                <Route path="/" element={<Dashboard />} />
                <Route path="/devices" element={<DeviceList />} />
                <Route path="/devices/:deviceId" element={<DeviceDetails />} />
                <Route path="/settings" element={<Settings />} />
                <Route path="*" element={<NotFound />} />
              </Routes>
            </Box>
          </Box>
        </Router>
      </ThemeProvider>
    </QueryClientProvider>
  );
}

export default App;
