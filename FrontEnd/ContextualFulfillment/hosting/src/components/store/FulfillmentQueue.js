import React                  from 'react'
import { bindActionCreators } from 'redux'
import { connect }            from 'react-redux'
import { withRouter }         from 'react-router-dom'

import { List,
         Grid,
         Typography }                 from 'material-ui'

import {  ExpandLess, 
          ExpandMore,
          Home }   from 'material-ui-icons'

import { withTheme}                from 'material-ui/styles'

import * as Actions         from '../../redux/actions'
import QueueCard          from './QueueCard'

var mapStateToProps = (state) => ({
  queue: state.fulfillmentQueue
})

class FulfillmentQueue extends React.Component {

  queueTitleLine = () => (
    <Typography align='left' variant="title" gutterBottom paragraph >
      Heep Queue
    </Typography>
  )
  
  render() {
    
    return (
        <Grid container justify='center' style={{marginTop: 20}}>
          <Grid item xs={10} style={{padding:24}}>
            <Grid container direction='column' alignItems='stretch' spacing={16}>
              {this.queueTitleLine()}
              <List>
                {Object.keys(this.props.queue).map((queueID)=> (
                    <QueueCard queueID={queueID} key={queueID}/>
                  ))}
              </List>
              
            </Grid>
          </Grid>
        </Grid>
    );
  
    
  }
}

var mapDispatchToProps = (dispatch) => {
  return bindActionCreators(Actions, dispatch)
}

export default withRouter(connect(mapStateToProps, mapDispatchToProps)(withTheme()(FulfillmentQueue)))