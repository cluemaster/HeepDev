import React from "react";
import { connect }            from 'react-redux'
import { bindActionCreators } from 'redux'
import { withRouter }         from 'react-router-dom'
import * as actions           from '../../redux/actions'
import PropTypes              from "prop-types";

import {  withStyles } from "material-ui/styles";
import {  Typography,
          Modal,
          Grid,
          FormControl,
          FormHelperText,
          Input,
          InputLabel,
          TextField,
          Button,
          ListItem,
          ListItemIcon,
          ListItemText } from "material-ui";
import { Add }  from 'material-ui-icons'

import VerticalStepper from '../utilities/VerticalStepper'


var mapStateToProps = (state) => ({
})

const styles = theme => ({
  paper: {
    position: "absolute",
    backgroundColor: theme.palette.background.paper,
    boxShadow: theme.shadows[5],
    padding: theme.spacing.unit * 4,
    minWidth: 500,
    top: '50%',
    left: '50%',
    transform: 'translate(-50%, -50%)'
  },
  formControl: {
    margin: theme.spacing.unit,
    minWidth: 120,
  },
  selectEmpty: {
    marginTop: theme.spacing.unit * 2,
  },
  textField: {
    marginLeft: theme.spacing.unit,
    marginRight: theme.spacing.unit,
    width: 200,
  },
});

class AddPlaceModal extends React.Component {
  state = {
    open: false,
    name: 'Home',
    ssid: '',
    password: ''
  };

  handleOpen = () => {
    this.setState({ open: true });
  };

  handleClose = () => {
    this.setState({ open: false });
  };

  handleChange = name => event => {
    this.setState({
      [name]: event.target.value,
    });
  };

  addPlaceListButton() {

    return (
      <ListItem button color='secondary' onClick={()=> this.setState({open: true})}>
        <ListItemIcon>
          <Add/>
        </ListItemIcon>
        <ListItemText inset secondary='Add a New Place' />
      </ListItem>
    )
  }

  modalTitle() {
    return (
      <Typography variant="title" align='center' color="inherit">
          Create a New Place
      </Typography>
    )
  };

  namePlaceForm() {

    return (
      <div>
        <form autoComplete='off'>
          <TextField
            id="name"
            label="Name"
            className={this.props.classes.textField}
            value={this.state.name}
            onChange={this.handleChange('name')}
            margin="normal"
          />
        </form>
      </div>
    )
  }

  placeCommsForm() {

    return (
      <div>
        <form autoComplete='off'>
          <TextField
            id="ssid"
            label="WiFi Network Name"
            className={this.props.classes.textField}
            value={this.state.ssid}
            onChange={this.handleChange('ssid')}
            margin="normal"
          />
          <TextField
            id="password"
            label="WiFi Network Password"
            className={this.props.classes.textField}
            value={this.state.password}
            onChange={this.handleChange('password')}
            margin="normal"
          />
        </form>
      </div>
    )
  }

  createPlaceForm() {


    const inputs = {
      stepper: {
        steps: [
          {
            title: 'Create a new place',
            description: `Let's create a new place for you. What would you like to name this place?`,
            form: this.namePlaceForm()
          },
          {
            title: 'How should Heep talk when on this network?',
            description: `Not all places have the same network topology. Save your wifi credentials to 
                          your place, and all future Heep Devices will be automatically logged in to your
                          network - it has never been so easy to install a new device!`,
            form: this.placeCommsForm()
          },
          {
            title: 'Associate devices with this place',
            description: `Now that we have your place set up, are there any devices you would like to add?`
          },
          {
            title:'Invite friends & Family',
            description: `Share this network with close friends and family - people you trust. They will be 
                          able to purchase new Heep devices for this place, and bring their own devices seamlessly
                          into this network.`
          }
        ],
        completionCallback: () => {
          this.props.saveNewPlace(this.state.name, this.state.ssid, this.state.password);
          this.handleClose();
        }
      }
    }

    return (
      <div>
        {this.modalTitle()}
        <VerticalStepper {...inputs.stepper}/>
      </div>
    )
  }

  render() {
    const { classes } = this.props;

    return (
      <div>
        {this.addPlaceListButton()}
        <Modal
          open={this.state.open}
          onClose={this.handleClose}>
          <div className={classes.paper}>
            {this.createPlaceForm()}
          </div>
        </Modal>
      </div>
    );
  }
}

AddPlaceModal.propTypes = {
  classes: PropTypes.object.isRequired
};


var mapDispatchToProps = (dispatch) => {
  return bindActionCreators(actions, dispatch)
}

export default withRouter(connect(mapStateToProps, mapDispatchToProps)(withStyles(styles)(AddPlaceModal)))