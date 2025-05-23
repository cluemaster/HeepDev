import React from 'react'
import { connect } from 'react-redux'
import { bindActionCreators } from 'redux'
import { withRouter } from 'react-router-dom'
import * as actions from '../redux/actions'
import * as auth from '../firebase/FirebaseAuth'

var mapStateToProps = (state) => ({
  loginStatus: state.loginStatus
})


class Logout extends React.Component {

  componentDidMount() {

    auth.logout();
  }

  render() {

    if (this.props.loginStatus) {
      
      return <div>Logging out...</div>

    } else {

      return <div>Logged Out!</div>
    }
  }
}

var mapDispatchToProps = (dispatch) => {
  return bindActionCreators(actions, dispatch)
}

export default withRouter(connect(mapStateToProps, mapDispatchToProps)(Logout))